/******************************************************************************/
/**
* @file Can.c
* @brief Implement Can
*
*******************************************************************************/

/* ***************** Header / include files ( #include ) **********************/
#include "Can.h"
#if defined (SELECT_CAN)
/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/* *********************** Global data definitions ****************************/
/* **************** Global constant definitions ( const ) *********************/
/* ***************** Modul global data segment ( static ) *********************/
/* *************** Modul global constants ( static const ) ********************/
/* **************** Local func/proc prototypes ( static ) *********************/
static uint16_t index = 0U;
static uint32_t TxPin = 0UL;
static uint32_t RxPin = 0UL;
static GPIO_TypeDef *pGPIO_CAN = NULL;
/******************************************************************************/
/**
* void CanInit(void)
* @brief Configure CAN 
*
*******************************************************************************/
void CanInit(tBSPType BSPType)
{	
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

//	switch(BSPType)
//	{
//		default:
			TxPin = BSP_BASE_CAN_TX_PIN;
			RxPin = BSP_BASE_CAN_RX_PIN;
			pGPIO_CAN = BSP_BASE_CAN_PORT;
//			break;
//	}
  
	pGPIO_CAN->AFR[TxPin >> 3] &= ~((uint32_t)MASK4 << (((uint32_t)TxPin & MASK3) << 2U));
	pGPIO_CAN->AFR[TxPin >> 3] |= ((uint32_t)GPIO_AF_1 << (((uint32_t)TxPin & MASK3) << 2U));

	pGPIO_CAN->AFR[RxPin >> 3] &= ~((uint32_t)MASK4 << (((uint32_t)RxPin & MASK3) << 2U));
	pGPIO_CAN->AFR[RxPin >> 3] |= ((uint32_t)GPIO_AF_1 << (((uint32_t)RxPin & MASK3) << 2U));

	pGPIO_CAN->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (TxPin << 1));
	pGPIO_CAN->OSPEEDR |= ((uint32_t)GPIO_Speed_Level_3 << (TxPin << 1));
	pGPIO_CAN->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)TxPin));
	pGPIO_CAN->OTYPER |= (uint16_t)(((uint16_t)GPIO_OType_PP) << ((uint16_t)TxPin));
	pGPIO_CAN->MODER &= ~(GPIO_MODER_MODER0 << (TxPin << 1));
	pGPIO_CAN->MODER |= ((uint32_t)GPIO_Mode_AF << (TxPin << 1));
	pGPIO_CAN->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (TxPin << 1));
	pGPIO_CAN->PUPDR |= ((uint32_t)GPIO_PuPd_UP << (TxPin << 1));

	pGPIO_CAN->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (RxPin << 1));
	pGPIO_CAN->OSPEEDR |= ((uint32_t)GPIO_Speed_Level_3 << (RxPin << 1));
	pGPIO_CAN->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)RxPin));
	pGPIO_CAN->OTYPER |= (uint16_t)(((uint16_t)GPIO_OType_PP) << ((uint16_t)RxPin));
	pGPIO_CAN->MODER &= ~(GPIO_MODER_MODER0 << (RxPin << 1));
	pGPIO_CAN->MODER |= ((uint32_t)GPIO_Mode_AF << (RxPin << 1));
	pGPIO_CAN->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (RxPin << 1));
	pGPIO_CAN->PUPDR |= ((uint32_t)GPIO_PuPd_UP << (RxPin << 1));

  RCC->APB1ENR |= RCC_APB1ENR_CANEN;
	
	CAN->MCR = CAN_MCR_INRQ;
	
  while((CAN->MSR & CAN_MSR_INAK) == 0);

  CAN->MCR |= CAN_MCR_NART;
  
	CAN->BTR = (2 << 20) | (3 << 16) | (1 << 0); // 500kbit at 8MHz

  CAN->FMR |= CAN_FMR_FINIT;
  CAN->FA1R &= ~CAN_FA1R_FACT0;  // deactivate filter
  CAN->FS1R |= CAN_FS1R_FSC0;  // set 32-bit scale configuration
  CAN->FM1R |= CAN_FM1R_FBM0;  // set two 32-bit identifier list mode
  CAN->sFilterRegister[0].FR1 = (CAN_ID_REV << 21);  // standard ID
  CAN->sFilterRegister[0].FR2 = (CAN_ID_REV << 21);
  CAN->FFA1R &= ~CAN_FFA1R_FFA0;  // assign filter to FIFO 0
  CAN->FA1R |= CAN_FA1R_FACT0;  // activate filter
  CAN->FMR &= ~CAN_FMR_FINIT;
  // Leave init mode

  CAN->MCR &= ~CAN_MCR_INRQ;
  while((CAN->MSR & CAN_MSR_INAK) != 0);
}

/******************************************************************************/
/**
* void CanSend(uint8_t *pTxData, uint16_t size)
* @brief Implement CAN send.
*
* @param[in] pTxData pointer to the data to be transmitted
* @param[in] size number of bytes
*
* CAN data format:
* LR                        HR
* D3 D2 D1 D0               D7 D6 D5 D4
*******************************************************************************/
void CanSend(uint8_t *pTxData, uint16_t size)
{
	uint16_t numRound = 0;
  uint32_t msgL = 0;
  uint32_t msgH = 0;
	uint16_t i = 0;
	uint8_t len = 0;
	
  while(size > 0)
  {
    len = min(size, CAN_MAX_DATA_LENGTH);
		
    for(i = 0; i < min(size, 4); i++)
    {
      msgL = (uint32_t)((pTxData[i + (numRound << 3U)] << (i << 3U)) | msgL);
    }
    for(i = min(size, 4); i < len; i++)
    {
      msgH = (uint32_t)((pTxData[i + (numRound << 3U)] << ((i - 4) << 3U)) | msgH);
    }
    while((CAN->TSR & CAN_TSR_TME0) == 0);  // wait until mailbox 0 is empty
    CAN->sTxMailBox[0].TIR = (CAN_ID_TRS << 21);
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
* eFUNCTION_RETURN CanRecv(uint8_t *pRxData, uint16_t size)
*
* @brief Read from UART. It will retry 3 times in case of failure.
*
* @param[out] pRxData pointer to 68 bytes data
* @param[in]  size number of bytes
* @returns    eFunction_Ok if successful
*             or
*             eFunction_Error if data losts.
*             or
*             eFunction_Timeout if an timeout error occurs.
*
*******************************************************************************/
eFUNCTION_RETURN CanRecv(uint8_t *pRxData, const uint16_t size)
{
	eFUNCTION_RETURN retVal = eFunction_Timeout;
	
	if(USART1->ISR & USART_ISR_RXNE)
	{
		pRxData[index] = USART1->RDR;
		index++;
	}
	
	if(index >= size)
	{
		index = 0;
		retVal = eFunction_Ok;
	}
  return retVal;
}

/******************************************************************************/
/**
* void CanReset(void)
*
* @brief Reset receive pointer index
*
* @returns    none
*
*******************************************************************************/
inline void CanReset(void)
{
	index = 0;
}

/******************************************************************************/
/**
* void CanDeInit(void)
*
* @brief Reset receive pointer index
*
* @returns    none
*
*******************************************************************************/
inline void CanDeInit(void)
{
	index = 0;
	RCC->APB2ENR &= ~(RCC_APB2ENR_USART1EN);
	
	USART1->CR1 &= ~(USART_CR1_TE | USART_CR1_RE | USART_CR1_UE);  
	
	pGPIO_CAN->AFR[TxPin >> 3] &= ~((uint32_t)MASK4 << (((uint32_t)TxPin & MASK3) << 2U));
	
	pGPIO_CAN->AFR[RxPin >> 3] &= ~((uint32_t)MASK4 << (((uint32_t)RxPin & MASK3) << 2U));
}
#endif
