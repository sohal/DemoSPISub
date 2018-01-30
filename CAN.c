/******************************************************************************/
/**
* @file Can.c
* @brief Implement Can
*
*******************************************************************************/

/* ***************** Header / include files ( #include ) **********************/
#include "Can.h"
#include "Gpio.h"
#include "Timeout.h"
#include "Command.h"
#include <string.h>

/* *************** Constant / macro definitions ( #define ) *******************/
#define CAN_MAX_DATA_LENGTH  8
#define CAN_ID_TRS           33
#define CAN_ID_REV           11

#define min(a,b) ((a)<(b)?(a):(b))

/* ********************* Type definitions ( typedef ) *************************/
/* *********************** Global data definitions ****************************/
/* **************** Global constant definitions ( const ) *********************/
/* ***************** Modul global data segment ( static ) *********************/
/* *************** Modul global constants ( static const ) ********************/
/* **************** Local func/proc prototypes ( static ) *********************/
#if defined(STM32F042x6)
static uint8_t firstMessage[CAN_MAX_DATA_LENGTH];
static uint8_t firstMessageLen;
static uint8_t firstMessageRead;
/******************************************************************************/
/**
* void CanInit(void)
* @brief Configure CAN (STM32F072_NUCLEO:PA11(RX),PA12(TX))
*                      (STM32F042: ?? ??) and initialze variables.
*
*******************************************************************************/
void CanInit(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIOA->AFR[GPIO_PIN_11 >> 3] &= ~((uint32_t)0xF << ((uint32_t)(((uint32_t)GPIO_PIN_11) & (uint32_t)0x07) * 4));
    GPIOA->AFR[GPIO_PIN_11 >> 3] |= ((uint32_t)(GPIO_AF_4) << ((uint32_t)(((uint32_t)GPIO_PIN_11) & (uint32_t)0x07) * 4));
    GPIOA->AFR[GPIO_PIN_12 >> 3] &= ~((uint32_t)0xF << ((uint32_t)(((uint32_t)GPIO_PIN_12) & (uint32_t)0x07) * 4));
    GPIOA->AFR[GPIO_PIN_12 >> 3] |= ((uint32_t)(GPIO_AF_4) << ((uint32_t)(((uint32_t)GPIO_PIN_12) & (uint32_t)0x07) * 4));

    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (GPIO_PIN_11 * 2));
    GPIOA->OSPEEDR |= ((uint32_t)GPIO_Speed_Level_3 << (GPIO_PIN_11 * 2));
    GPIOA->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)GPIO_PIN_11));
    GPIOA->OTYPER |= (uint16_t)(((uint16_t)GPIO_OType_PP) << ((uint16_t)GPIO_PIN_11));
    GPIOA->MODER &= ~(GPIO_MODER_MODER0 << (GPIO_PIN_11 * 2));
    GPIOA->MODER |= ((uint32_t)GPIO_Mode_AF << (GPIO_PIN_11 * 2));
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (GPIO_PIN_11 * 2));
    GPIOA->PUPDR |= ((uint32_t)GPIO_PuPd_NOPULL << (GPIO_PIN_11 * 2));

    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (GPIO_PIN_12 * 2));
    GPIOA->OSPEEDR |= ((uint32_t)GPIO_Speed_Level_3 << (GPIO_PIN_12 * 2));
    GPIOA->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)GPIO_PIN_12));
    GPIOA->OTYPER |= (uint16_t)(((uint16_t)GPIO_OType_PP) << ((uint16_t)GPIO_PIN_12));
    GPIOA->MODER &= ~(GPIO_MODER_MODER0 << (GPIO_PIN_12 * 2));
    GPIOA->MODER |= ((uint32_t)GPIO_Mode_AF << (GPIO_PIN_12 * 2));
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (GPIO_PIN_12 * 2));
    GPIOA->PUPDR |= ((uint32_t)GPIO_PuPd_NOPULL << (GPIO_PIN_12 * 2));

    RCC->APB1ENR |= RCC_APB1ENR_CANEN;

    // Enter init mode

    CAN->MCR = CAN_MCR_INRQ;
    while((CAN->MSR & CAN_MSR_INAK) == 0);

    CAN->MCR |= CAN_MCR_NART;
    //CAN->BTR = (2 << 20) | (3 << 16) | (7 << 0); // 125kbit at 8MHz
    CAN->BTR = (2 << 20) | (3 << 16) | (1 << 0); // 500kbit at 8MHz

    CAN->FMR |= CAN_FMR_FINIT;
    CAN->FA1R &= ~CAN_FA1R_FACT0;  // deactivate filter
    CAN->FS1R |= CAN_FS1R_FSC0;  // set 32-bit scale configuration
    CAN->FM1R |= CAN_FM1R_FBM0;  // set two 32-bit identifier list mode
    CAN->sFilterRegister[0].FR1 = (CAN_ID_REV << 21);  // standard ID
    CAN->sFilterRegister[0].FR2 = (CAN_ID_REV << 21);
//    CAN->sFilterRegister[0].FR1 = (CAN_ID_REV << 3) | (1 << 2);  // extended ID
//    CAN->sFilterRegister[0].FR2 = (CAN_ID_REV << 3) | (1 << 2);
    CAN->FFA1R &= ~CAN_FFA1R_FFA0;  // assign filter to FIFO 0
    CAN->FA1R |= CAN_FA1R_FACT0;  // activate filter
    CAN->FMR &= ~CAN_FMR_FINIT;
  // Leave init mode

    CAN->MCR &= ~CAN_MCR_INRQ;
    while((CAN->MSR & CAN_MSR_INAK) != 0);
    memset(firstMessage, 0, CAN_MAX_DATA_LENGTH);
    firstMessageLen = 0;
    firstMessageRead = 0;
}

/******************************************************************************/
/**
* void CanTransmit(uint8_t *pTxData, uint16_t size)
* @brief Implement CAN send.
*
* @param[in] pTxData pointer to the data to be transmitted
* @param[in] size number of bytes
*
* CAN data format:
* LR                        HR
* D3 D2 D1 D0               D7 D6 D5 D4
*******************************************************************************/
void CanTransmit(uint8_t *pTxData, uint16_t size)
{
    uint16_t numRound = 0;
    uint32_t msgL = 0;
    uint32_t msgH = 0;
    while(size > 0)
    {
        uint16_t i;
        uint8_t len = min(size, CAN_MAX_DATA_LENGTH);
        for(i = 0; i < min(size, 4); i++)
        {
            msgL = (uint32_t)((pTxData[i + (numRound * 8)] << (i * 8)) | msgL);
        }
        for(i = min(size, 4); i < len; i++)
        {
            msgH = (uint32_t)((pTxData[i + (numRound * 8)] << ((i - 4) * 8)) | msgH);
        }
        while((CAN->TSR & CAN_TSR_TME0) == 0);  // wait until mailbox 0 is empty
        CAN->sTxMailBox[0].TIR = (CAN_ID_TRS << 21);
//    CAN->sTxMailBox[0].TIR = (CAN_ID_TRS << 3) | CAN_TI0R_IDE;
        CAN->sTxMailBox[0].TDLR = msgL;
        CAN->sTxMailBox[0].TDHR = msgH;
        CAN->sTxMailBox[0].TDTR &= ~CAN_TDT0R_DLC;
        CAN->sTxMailBox[0].TDTR |= len;
        CAN->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;  // start transmission
        while((CAN->TSR & CAN_TSR_RQCP0) == 0);  // wait until transmission completed
        CAN->TSR |= CAN_TSR_RQCP0;
        size -= len;
        numRound++; 
    }
}

/******************************************************************************/
/**
* eFUNCTION_RETURN CanReceive(uint8_t *pRxData, uint16_t size)
*
* @brief Read from CAN.
*
* @param[out] pRxData pointer to data to receive
* @param[in]  size number of bytes
* @returns    eFunction_Ok if successful
*             or
*             eFunction_Error if timeout error or message id/length error occurs.
*
* CAN data format:
* LR                        HR
* D3 D2 D1 D0               D7 D6 D5 D4
*******************************************************************************/
eFUNCTION_RETURN CanReceive(uint8_t *pRxData, uint16_t size)
{
    uint16_t sizeLoc = size;
    uint8_t *pRxDataLoc = pRxData;
    uint8_t retryCnt = 0; // retry counter, if it reachs 3, return timeout
    uint8_t DataReceived = 0;
    uint32_t id;
    uint8_t len;
    uint32_t msgL;
    uint32_t msgH;
    uint8_t msgLoc[CAN_MAX_DATA_LENGTH];
    memset(msgLoc, 0, CAN_MAX_DATA_LENGTH);
    if (firstMessageRead)
    {
        memcpy(pRxDataLoc, firstMessage, firstMessageLen);
        pRxDataLoc += firstMessageLen;
        sizeLoc -= firstMessageLen;
        size -= firstMessageLen;
        pRxData += firstMessageLen;
        firstMessageRead = 0;
        DataReceived = 1;
    }
    while(sizeLoc > 0)
    {
        uint32_t i = 0;
        while((i++ < PACKET_TIMEOUT_CAN_MS) && ((CAN->RF0R & CAN_RF0R_FMP0) == 0));
        if(CAN->RF0R & CAN_RF0R_FMP0)
        {
            if((CAN->sFIFOMailBox[0].RIR & CAN_RI0R_IDE) == 0)
            {
                id = CAN->sFIFOMailBox[0].RIR >> 21;
            }
            else
            {
                id = CAN->sFIFOMailBox[0].RIR >> 3;
            }
            if(id != CAN_ID_REV)
            {
                return eFunction_Error;
            }
            len = CAN->sFIFOMailBox[0].RDTR & 0xf; // current message length
            msgL = CAN->sFIFOMailBox[0].RDLR;
            msgH = CAN->sFIFOMailBox[0].RDHR;
            CAN->RF0R |= CAN_RF0R_RFOM0;
            msgLoc[0] = msgL & 0x000000FF;
            msgLoc[1] = (msgL >> 8) & 0x000000FF;
            msgLoc[2] = (msgL >> 16) & 0x000000FF;
            msgLoc[3] = (msgL >> 24) & 0x000000FF;
            msgLoc[4] = msgH & 0x000000FF;
            msgLoc[5] = (msgH >> 8) & 0x000000FF;
            msgLoc[6] = (msgH >> 16) & 0x000000FF;
            msgLoc[7] = (msgH >> 24) & 0x000000FF;
            memcpy(pRxDataLoc, msgLoc, len); // only copy the real length, the rest is rubbish
            pRxDataLoc += len;
            retryCnt = 0;
            sizeLoc -= len;
            DataReceived = 1;
        }
        else
        {
            sizeLoc = size;
            pRxDataLoc = pRxData;
            CAN->RF0R &= ~CAN_RF0R_FOVR0; // clear overrun flag
            if(retryCnt++ >= 3)
            {
                if (DataReceived)
                {
                    // data lost
                    return eFunction_Error;
                }
                else
                {
                    // polling timeout
                    return eFunction_Timeout;
                }
            }
        }
    }
    return eFunction_Ok;
}

/******************************************************************************/
/**
* uint8_t CanMsgReceived(void)
*
* @brief Check if there is any data on bus. Read the first CAN message out.
*        The message size can be up to 8 bytes. Only the first firstMessageLen
*        bytes are the valid data.
*
* @returns    1 if there is message on bus.
*             or
*             0 if there is nothing.
*
*******************************************************************************/
uint8_t CanMsgReceived(void)
{
    uint32_t msgL;
    uint32_t msgH;
    uint32_t id;
    if(CAN->RF0R & CAN_RF0R_FMP0)
    {
        if((CAN->sFIFOMailBox[0].RIR & CAN_RI0R_IDE) == 0)
        {
            id = CAN->sFIFOMailBox[0].RIR >> 21;
        }
        else
        {
            id = CAN->sFIFOMailBox[0].RIR >> 3;
        }
        if(id != CAN_ID_REV)
        {
            return 0;
        }
        firstMessageLen = CAN->sFIFOMailBox[0].RDTR & 0xf;
        msgL = CAN->sFIFOMailBox[0].RDLR;
        msgH = CAN->sFIFOMailBox[0].RDHR;
        CAN->RF0R |= CAN_RF0R_RFOM0;
        firstMessage[0] = msgL & 0x000000FF;
        firstMessage[1] = (msgL >> 8) & 0x000000FF;
        firstMessage[2] = (msgL >> 16) & 0x000000FF;
        firstMessage[3] = (msgL >> 24) & 0x000000FF;
        firstMessage[4] = msgH & 0x000000FF;
        firstMessage[5] = (msgH >> 8) & 0x000000FF;
        firstMessage[6] = (msgH >> 16) & 0x000000FF;
        firstMessage[7] = (msgH >> 24) & 0x000000FF;
        firstMessageRead = 1;
        return 1;
    }
    else
    {
        return 0;
    }
}
#endif
