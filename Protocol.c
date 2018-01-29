/******************************************************************************/
/**
* @file Protocol.c
* @brief Implementation of the protocol layer
*
*******************************************************************************/

/* ***************** Header / include files ( #include ) **********************/
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#if defined(STM32F031x6)
    #include "Usart1.h"
    #include "Spi1.h"
#elif defined(STM32F042x6)
    #include "Can.h"
#else 
    #error "CPU type needs to be specified based on the device, either STMF031K6 or STM32F042K6"
#endif
#include "Protocol.h"
#include "Packet.h"
#include "Timeout.h"
#include "Timer.h"
#include "Flash.h"
#include "Gpio.h"

/* *************** Constant / macro definitions ( #define ) *******************/
#define SRAM_START_ADDRESS  0x20000000

/* ********************* Type definitions ( typedef ) *************************/
/* *********************** Global data definitions ****************************/
/* **************** Global constant definitions ( const ) *********************/
/* ***************** Modul global data segment ( static ) *********************/
static tDATA_PACKET dataReceived;
static tFIRMWARE_PARAM firmwareParam;
static eSTATES state;
#if defined(STM32F031x6)
static eCOMMUNICATION_INTERFACE comIF;
#endif

/* *************** Modul global constants ( static const ) ********************/
/* **************** Local func/proc prototypes ( static ) *********************/
static eFUNCTION_RETURN ProtocolReadCMD(eCOMMAND_ID* cmd);
static eFUNCTION_RETURN ProtocolSendResponse(eRESPONSE_ID res);
static void ProtocolDelay3ms(void);
static void ProtocolStartApp(void);

/******************************************************************************/
/**
* void ProtocolInit(void)
* @brief Initialise the protocol function.
*
*******************************************************************************/
void ProtocolInit(void)
{
    state = eSTATE_WaitForCmd;
    memset(&firmwareParam, 0, sizeof(tFIRMWARE_PARAM));
    memset(&dataReceived, 0, sizeof(tDATA_PACKET));
#if defined(STM32F031x6)
    comIF = GpioGetComIF();
#endif
}
/******************************************************************************/
/**
* void ProtocolStateProcess(void)
* @brief State machine for handling the input command.
*
*******************************************************************************/
void ProtocolStateProcess(void)
{
    eCOMMAND_ID input = eCMD_NotValid; /* the input 2 bytes command from UART */
    eFUNCTION_RETURN ret;
    static uint8_t isFinishCmd = 0;
    switch(state)
    {
        case eSTATE_WaitForCmd:
            isFinishCmd = 0;
            ret = ProtocolReadCMD(&input);
            if((ret == eFunction_Ok) && (input == eCMD_BootloadMode))
            {
                state = eSTATE_BootloaderMode;
                ProtocolDelay3ms();
                ProtocolSendResponse(eRES_Ready);
            }
            if(TimerIsTimeout(WAIT_CMD_TIMEOUT_S))
            {
                state = eSTATE_VerifyCRC;
            }
            break;
        case eSTATE_BootloaderMode:
            ret = ProtocolReadCMD(&input);
            if(ret == eFunction_Ok)
            {
                if(input == eCMD_EraseFlash)
                {
                    FlashErase(); /* Erase flash */
                    if(FlashIsErased())
                    {
                        ProtocolSendResponse(eRES_OK);
                    }
                }
                else if(input == eCMD_WriteMemory)
                {
                    ProtocolDelay3ms();
                    if(FlashIsErased())
                    {
                        state = eSTATE_WaitForPacket;
                        TimerStartTimeout();
                        PacketInit();
                        ProtocolSendResponse(eRES_OK);
                    }
                    else
                    {
                        ProtocolSendResponse(eRES_Error);
                    }
                    // start Bootloader Timeout
                }
                else if(input == eCMD_WriteCRC)
                {
                    state = eSTATE_WriteCRC;
                    ProtocolSendResponse(eRES_OK);
                }
                else if(input == eCMD_Finish)
                {
                    isFinishCmd = 1;
                    ProtocolDelay3ms();
                    state = eSTATE_VerifyCRC;
                }
                else
                {
                    // do nothing, stay in bootloader mode
                }
            }
            break;
        case eSTATE_WaitForPacket:
#if defined(STM32F031x6)
            if(comIF == eSPI)
            {
                if (Spi1ByteReceived())
                {
                    state = eSTATE_ReceivePacket;
                }
            }
            else //if(comIF == eUSART)
            {
                if (Usart1ByteReceived())
                {
                    state = eSTATE_ReceivePacket;
                }
            }
#elif defined(STM32F042x6)
            //CAN interface
            if (CanMsgReceived())
            {
                state = eSTATE_ReceivePacket;
            }
#endif
            if(TimerIsTimeout(BOOTLOADER_TIMEOUT_S))
            {
                state = eSTATE_BootloaderMode;
            }
            break;
        case eSTATE_ReceivePacket:
#if defined(STM32F031x6)
            if(comIF == eSPI)
            {
                ret = Spi1Receive(&dataReceived.u8Data[0], sizeof(tDATA_PACKET));
            }
            else //if(comIF == eUSART)
            {
                ret = Usart1Receive(&dataReceived.u8Data[0], sizeof(tDATA_PACKET));
            }
#elif defined(STM32F042x6)
            //CAN interface
            ret = CanReceive(&dataReceived.u8Data[0], sizeof(tDATA_PACKET));
#endif
            if(ret == eFunction_Ok)
            {
                /* 68 bytes data should be now in place */
                // process it, if also fine:
                if(PacketProcess(dataReceived) == ePACKET_Ok)
                {
                    state = eSTATE_WritePacket;
                }
                else 
                {
                    state = eSTATE_Error;
                }
            }
            else
            {
                state = eSTATE_Error;
            }
            break;
        case eSTATE_WriteCRC:
#if defined(STM32F031x6)
            if(comIF == eSPI)
            {
                ret = Spi1Receive((uint8_t *)&firmwareParam.u16FWCRC, sizeof(tFIRMWARE_PARAM));
            }
            else //if(comIF == eUSART)
            {
                ret = Usart1Receive((uint8_t *)&firmwareParam.u16FWCRC, sizeof(tFIRMWARE_PARAM));
            }
#elif defined(STM32F042x6)
            //CAN interface
            ret = CanReceive((uint8_t *)&firmwareParam.u16FWCRC, sizeof(tFIRMWARE_PARAM));
#endif
            if(ret == eFunction_Ok)
            {
                /* Write to a fixed Flash address */
                if(FlashWriteFWParam(firmwareParam))
                {
                    // verify immediately
                    if(FlashVerifyFWParam(firmwareParam))
                    {
                        ProtocolSendResponse(eRES_OK);
                    }
                    else
                    {
                        ProtocolSendResponse(eRES_Abort);
                    }
                    state = eSTATE_BootloaderMode;
                }
              
            }
            else
            {
                state = eSTATE_Error;
            }
            break;
        case eSTATE_OKtoSender:
            // write OK to sender and go to eSTATE_WaitForPacket
            ProtocolSendResponse(eRES_OK);
            state = eSTATE_WaitForPacket;
            TimerStartTimeout();
            break;
        case eSTATE_WritePacket:
            // write to flash
            if(FlashWrite(&dataReceived.u8Data[0], BLOCK_SIZE))
            {
                // verify immediately
                if(FlashVerify(&dataReceived.u8Data[0], BLOCK_SIZE))
                {
                    state = eSTATE_OKtoSender;
                }
                else
                {
                    ProtocolSendResponse(eRES_Abort);
                    state = eSTATE_BootloaderMode;
                }
            }
            //else // something wrong during write to flash
            {
                // 1 retry write to flash
                //if(ARM_Flash_ProgramData(addr, dataReceived.u8Data[0], 64))
                {
                    //state = eSTATE_OKtoSender;
                }
                //else
                {
                    //ProtocolSendResponse(eRES_Error);
                    //state = eSTATE_ExitBootloader;
                }
            }
            break;
        case eSTATE_VerifyCRC:
            if(FlashVerifyFirmware())
            {
                state = eSTATE_ExitBootloader;
                if(1 == isFinishCmd) /* Only send response if it comes from Finish command */
                {
                    ProtocolSendResponse(eRES_OK);
                }
            }
            else
            {
                /* Stay in Bootloader mode if it fails the verification */
                state = eSTATE_BootloaderMode;
                if(1 == isFinishCmd) /* Only send response if it comes from Finish command */
                {
                    ProtocolSendResponse(eRES_AppCrcErr);
                }
            }
            break;
        case eSTATE_Error:
            state = eSTATE_WaitForPacket;
            ProtocolSendResponse(eRES_Error);
            TimerStartTimeout();
            break;
        case eSTATE_ExitBootloader:
            FlashLock();
            // start application
            ProtocolStartApp();
            break;
        default:
            break;
    }
}

/******************************************************************************/
/**
* static void ProtocolStartApp(void)
* @brief Copy execption vector table from application to RAM and jump to reset
*        handler.
*
*******************************************************************************/
static void ProtocolStartApp(void)
{
    extern uint32_t __Vectors_Size;
    volatile uint32_t *__vectors = (volatile uint32_t *)FLASH_PROGRAM_START_ADDRESS;
    volatile uint32_t *__ram = (volatile uint32_t *)SRAM_START_ADDRESS;
    TimerDeInit();
    for(int i = 0; i < ((uint32_t)&__Vectors_Size / sizeof(uint32_t)); i++)
    {
        __ram[i] = __vectors[i];
    }

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_MEM_MODE;  // remap exception vectors

    __set_MSP(*__vectors);  // setup stack pointer
    ((void (*)(void))*(__vectors + 1))();  // jump to reset handler
}

/******************************************************************************/
/**
* static eFUNCTION_RETURN ProtocolReadCMD(eCOMMAND_ID* cmd)
*
* @brief Read 2 bytes from UART or SPI (polling) and construct a command.
*
* @param[in/out] cmd pointer to 2 bytes command
* @returns    eFunction_Ok if successful
*             or
*             eFunction_Timeout if an timeout error occurs.
*
*******************************************************************************/
static eFUNCTION_RETURN ProtocolReadCMD(eCOMMAND_ID* cmd)
{
    uint8_t dataRx[2] = {0x00, 0x00};
#if defined(STM32F031x6)
    if(comIF == eSPI)
    {
        Spi1Receive(dataRx, 2);
    }
    else
    {
        Usart1Receive(dataRx, 2);
    }
#elif defined(STM32F042x6)
    CanReceive(dataRx, 2);
#endif
    *cmd = (eCOMMAND_ID)(((uint16_t)dataRx[0] << 8) | dataRx[1]);
    return eFunction_Ok;
}

/******************************************************************************/
/**
* static eFUNCTION_RETURN ProtocolSendResponse(eRESPONSE_ID* res)
*
* @brief Write 2 bytes response, for UART interface eFunction_Ok is returned;
*        For SPI interface, there could be a timeout due to the nature of SPI.
*
* @param[in/out] res pointer to 2 bytes response
* @returns    eFunction_Ok if successful
*             or
*             eFunction_Timeout if an timeout error occurs.
*
*******************************************************************************/
static eFUNCTION_RETURN ProtocolSendResponse(eRESPONSE_ID res)
{
    eFUNCTION_RETURN ret = eFunction_Ok;
    uint8_t dataTx[2];
    dataTx[0] = (res >> 8) & 0x00FF; // MSB
    dataTx[1] = res & 0x00FF; // LSB
#if defined(STM32F031x6)
    if(comIF == eSPI)
    {
        ret = Spi1Transmit(&dataTx[0], 2);
    }
    else
    {
        Usart1Transmit(&dataTx[0], 2);
    }
#elif defined(STM32F042x6)
    //via CAN
    CanTransmit(&dataTx[0], 2);
#endif
    return ret;
}

/******************************************************************************/
/**
* static void ProtocolDelay3ms(void)
*
* @brief Function to delay about 3 ms.
*
*******************************************************************************/
static void ProtocolDelay3ms(void)
{
    for(uint16_t i = 0; i < 3000u; i++){}
}    
