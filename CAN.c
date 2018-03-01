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
	uint32_t canWait;
	
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  TxPin = BSP_BASE_CAN_TX_PIN;
  RxPin = BSP_BASE_CAN_RX_PIN;
  pGPIO_CAN = BSP_BASE_CAN_PORT;
  
	pGPIO_CAN->AFR[TxPin >> 3] &= ~((uint32_t)MASK4 << (((uint32_t)TxPin & MASK3) << 2U));
	pGPIO_CAN->AFR[TxPin >> 3] |= ((uint32_t)GPIO_AF_4 << (((uint32_t)TxPin & MASK3) << 2U));

	pGPIO_CAN->AFR[RxPin >> 3] &= ~((uint32_t)MASK4 << (((uint32_t)RxPin & MASK3) << 2U));
	pGPIO_CAN->AFR[RxPin >> 3] |= ((uint32_t)GPIO_AF_4 << (((uint32_t)RxPin & MASK3) << 2U));

	pGPIO_CAN->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (TxPin << 1));
	pGPIO_CAN->OSPEEDR |= ((uint32_t)GPIO_Speed_Level_3 << (TxPin << 1));
	pGPIO_CAN->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)TxPin));
	pGPIO_CAN->OTYPER |= (uint16_t)(((uint16_t)GPIO_OType_PP) << ((uint16_t)TxPin));
	pGPIO_CAN->MODER &= ~(GPIO_MODER_MODER0 << (TxPin << 1));
	pGPIO_CAN->MODER |= ((uint32_t)GPIO_Mode_AF << (TxPin << 1));
	pGPIO_CAN->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (TxPin << 1));
	pGPIO_CAN->PUPDR |= ((uint32_t)GPIO_PuPd_NOPULL << (TxPin << 1));

	pGPIO_CAN->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (RxPin << 1));
	pGPIO_CAN->OSPEEDR |= ((uint32_t)GPIO_Speed_Level_3 << (RxPin << 1));
	pGPIO_CAN->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)RxPin));
	pGPIO_CAN->OTYPER |= (uint16_t)(((uint16_t)GPIO_OType_PP) << ((uint16_t)RxPin));
	pGPIO_CAN->MODER &= ~(GPIO_MODER_MODER0 << (RxPin << 1));
	pGPIO_CAN->MODER |= ((uint32_t)GPIO_Mode_AF << (RxPin << 1));
	pGPIO_CAN->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (RxPin << 1));
	pGPIO_CAN->PUPDR |= ((uint32_t)GPIO_PuPd_NOPULL << (RxPin << 1));
	
	/** Turn on clock for CAN */
	RCC->APB1ENR |= RCC_APB1ENR_CANEN;
	/** Enter CAN init mode to write configuration */
	CAN->MCR |= CAN_MCR_INRQ;
	/** Wait until we enter init mode */
	/** Setup busy wait timer */
	canWait = BootTIMEOUT;
  while((CAN->MSR & CAN_MSR_INAK) == 0)
	{
		if(!(canWait--))
		{
			return;		/** Return if the busy wait timer expires todo needs a proper error code */						
		}	
	}
	/** Exit sleep mode */
	CAN->MCR &= ~CAN_MCR_SLEEP;
  /** loopback mode set timing to 500kBaud: BS1 = 4, BS2 = 3, prescaler = 2 */
  CAN->BTR |= (2 << 20) | (3 << 16) | (1 << 0); 
	/** Activate filter 0 */
	CAN->FMR |= CAN_FMR_FINIT;
	/** Set the ID and mask (all bits of std id care */
	CAN->FA1R |= CAN_FA1R_FACT0;
	/** Set the ID and the mask */
	CAN->sFilterRegister[0].FR1 = (BSP_BASE_CAN_ID << 5) | (0xFF70U <<16);
	/** Leave filter init */
	CAN->FMR &= ~CAN_FMR_FINIT;
	/** Set FIFO0 message pending IT enabled */
	CAN->IER |= CAN_IER_FMPIE0;
	/** Come out of initialization */
  CAN->MCR &= ~CAN_MCR_INRQ;
	/** Wait until we exit init mode */
	/** Setup busy wait timer */
	canWait = BootTIMEOUT;
  while((CAN->MSR & CAN_MSR_INAK) != 0)
	{		
		if(!(canWait--))
		{
			return;		/** Return if the busy wait timer expires */						
		}	
	}
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
void CanSend(uint8_t *pTxData, const uint16_t size)
{
	uint32_t i;
	tCANDataUnion TwoUnion[2];
	uint16_t tempindex = 0;
	uint16_t loop8Bytes;
	uint32_t	canWait;

	loop8Bytes = size / CAN_MAX_DATA_LENGTH;
	if(size % CAN_MAX_DATA_LENGTH)
	{
		loop8Bytes += 1;
	}
		
	while(loop8Bytes)
	{
		TwoUnion[0].uint4Bytes = 0U;
		TwoUnion[1].uint4Bytes = 0U;
	
		for(i = 0; i < CAN_MAX_DATA_LENGTH; i++)
		{
			if(tempindex >= size)
			{
				break;
			}else
			{
				if(i < 4)
					TwoUnion[0].uint1Byte[i] = pTxData[tempindex++];
				else
					TwoUnion[1].uint1Byte[i-4] = pTxData[tempindex++];
			}
		}
		/** Setup busy wait timer */
		canWait = BootTIMEOUT;
		while((CAN->TSR & CAN_TSR_TME0) == 0)
		{
			if(!(canWait--))
			{
				return;		/** Return if the busy wait timer expires */						
			}
		}
		CAN->sTxMailBox[0].TDLR = TwoUnion[0].uint4Bytes;
		CAN->sTxMailBox[0].TDHR = TwoUnion[1].uint4Bytes;

		CAN->sTxMailBox[0].TIR = (BSP_BASE_CAN_ID << 21);

		CAN->sTxMailBox[0].TDTR &= ~CAN_TDT0R_DLC;

		CAN->sTxMailBox[0].TDTR |= size;

		CAN->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;  
		/** Setup busy wait timer for transmission */
		canWait = BootTIMEOUT;
		while((CAN->TSR & CAN_TSR_RQCP0) == 0)
		{
			if(!(canWait--))
			{
				return;		/** Return if the busy wait timer expires */						
			}
		}
		
		CAN->TSR |= CAN_TSR_RQCP0;
		loop8Bytes--;
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
	uint32_t	temp32U = 0U;
	uint32_t	i;
	tCANDataUnion TwoUnion[2];
	
	if((CAN->RF0R & CAN_RF0R_FMP0) != 0U)
	{
		/** Read the FIFO */
		TwoUnion[0].uint4Bytes = CAN->sFIFOMailBox[0].RDLR;
		TwoUnion[1].uint4Bytes = CAN->sFIFOMailBox[0].RDHR;
		/** Get the ID from the receive mailbox fifo */
		if((CAN->sFIFOMailBox[0].RIR & CAN_RI0R_IDE) == 0)
		{
			temp32U = CAN->sFIFOMailBox[0].RIR >> 21U;
		}else
		{
			temp32U = CAN->sFIFOMailBox[0].RIR >> 3U;
		}
		/** Check if ID is matching our ID */
		if(temp32U != BSP_BASE_CAN_ID)
		{
			// retVal = eFunction_Error;
		}else
		{
			/** Get the current message length */
			temp32U = CAN->sFIFOMailBox[0].RDTR & 0x000FU; // Get the DLC [3:0]

			for(i = 0; i < CAN_MAX_DATA_LENGTH; i++)
			{
				if(i < 4)
				{
					pRxData[index] = TwoUnion[0].uint1Byte[i];
				}else
				{
					pRxData[index] = TwoUnion[1].uint1Byte[i];
				}
				index++;
				if(index >= size)
				{
					index = 0;
					retVal = eFunction_Ok;
					break;
				}
			}
		}
		/** Release FIFO */
		CAN->RF0R |= CAN_RF0R_RFOM0;
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
	RCC->APB1ENR &= ~(RCC_APB1ENR_CANEN);
	
	pGPIO_CAN->AFR[TxPin >> 3] &= ~((uint32_t)MASK4 << (((uint32_t)TxPin & MASK3) << 2U));
	
	pGPIO_CAN->AFR[RxPin >> 3] &= ~((uint32_t)MASK4 << (((uint32_t)RxPin & MASK3) << 2U));
}
#endif
