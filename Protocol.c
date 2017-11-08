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
#include "Usart1.h"
#include "Protocol.h"
#include "Packet.h"
#include "Timeout.h"
#include "Timer.h"
#include "Flash.h"
/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/* *********************** Global data definitions ****************************/
/* **************** Global constant definitions ( const ) *********************/
/* ***************** Modul global data segment ( static ) *********************/
static tDATA_PACKET dataReceived;
static eSTATES state;

/* *************** Modul global constants ( static const ) ********************/
/* **************** Local func/proc prototypes ( static ) *********************/
static eFUNCTION_RETURN ProtocolReadCMD(eCOMMAND_ID* cmd);
static eFUNCTION_RETURN ProtocolSendResponse(eRESPONSE_ID res);

/******************************************************************************/
/**
* void ProtocolInit(void)
* @brief Initialise the protocol function.
*
*******************************************************************************/
void ProtocolInit(void)
{
    state = eSTATE_WaitForCmd;
    memset(&dataReceived, 0, sizeof(dataReceived));
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
    switch(state)
    {
        case eSTATE_WaitForCmd:
            ret = ProtocolReadCMD(&input);
            if((ret == eFunction_Ok) && (input == eCMD_BootloadMode))
            {
                state = eSTATE_BootloaderMode;
                ProtocolSendResponse(eRES_Ready);
            }
            if(TimerIsTimeout(WAIT_CMD_TIMEOUT_S))
            {
                state = eSTATE_ExitBootloader;
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
                else if(input == eCMD_Finish)
                {
                    ProtocolSendResponse(eRES_OK);
                    state = eSTATE_ExitBootloader;
                }
                else
                {
                    // do nothing, stay in bootloader mode
                }
            }
            break;
        case eSTATE_WaitForPacket:
            if (Usart1ByteReceived())
            {
                state = eSTATE_ReceivePacket;
            }  
            if(TimerIsTimeout(BOOTLOADER_TIMEOUT_S))
            {
                state = eSTATE_BootloaderMode;
            }
            break;
        case eSTATE_ReceivePacket:
            ret = Usart1Receive(&dataReceived.u8Data[0], 68);
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
        case eSTATE_OKtoSender:
            // uart write OK to sender and go to eSTATE_WaitForPacket
            ProtocolSendResponse(eRES_OK);
            state = eSTATE_WaitForPacket;
            TimerStartTimeout();
            break;
        case eSTATE_WritePacket:
            // write to flash
            if(FlashWrite(&dataReceived.u8Data[0]))
            {
                // verify immediately
                if(FlashVerify(&dataReceived.u8Data[0]))
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
        case eSTATE_Error:
            state = eSTATE_WaitForPacket;
            ProtocolSendResponse(eRES_Error);
            TimerStartTimeout();
            break;
        case eSTATE_ExitBootloader:
            FlashLock();
            // start application()
            break;
        default:
            break;
    }
}

/******************************************************************************/
/**
* static eFUNCTION_RETURN ProtocolReadCMD(eCOMMAND_ID* cmd)
*
* @brief Read 2 bytes from UART (polling) and construct a command.
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
    Usart1Receive(dataRx, 2);
    *cmd = (eCOMMAND_ID)(((uint16_t)dataRx[0] << 8) | dataRx[1]);
    return eFunction_Ok;
}

/******************************************************************************/
/**
* static eFUNCTION_RETURN ProtocolSendResponse(eRESPONSE_ID* res)
*
* @brief Write 2 bytes response to UART.
*
* @param[in/out] res pointer to 2 bytes response
* @returns    eFunction_Ok if successful
*             or
*             eFunction_Timeout if an timeout error occurs.
*
*******************************************************************************/
static eFUNCTION_RETURN ProtocolSendResponse(eRESPONSE_ID res)
{
    uint8_t dataTx[2];
    dataTx[0] = (res >> 8) & 0x00FF; // MSB
    dataTx[1] = res & 0x00FF; // LSB
    Usart1Transmit(&dataTx[0], 2);
    return eFunction_Ok;
}