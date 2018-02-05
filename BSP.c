#include "BSP.h"

static tBSPStruct gIF;

tBSPStruct* BSP_Init(void)
{
	gIF.pInit 		= NULL;
	gIF.pDeInit 	= NULL;
	gIF.pSend 		= NULL;
	gIF.pReset 		= NULL;
	gIF.BSP_Type 	= BSP_UnKnown;

#if 0
	uint32_t temp_u32;
	temp_u32 = DBGMCU->IDCODE;
	
	if((temp_u32 & DBGMCU_IDCODE_DEV_ID) != DBGMCU_ID_F03x)
	{
		/* Make BSP config for F04x, Interface is always CAN */
		gIF.BSP_Type = BSP_STM32F042;
	}else
	{
		/* Interface can be SPI or USART for F03x family so more investigation necessary */
	
		RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
		/* Input pin 6 is made active */
		BSP_CHECK_PORT->MODER &= ~(GPIO_MODER_MODER0 << (BSP_CHECK_PIN_6 * 2));
		BSP_CHECK_PORT->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (BSP_CHECK_PIN_6 * 2));
		/* Input pin 6 is made active */
		BSP_CHECK_PORT->MODER &= ~(GPIO_MODER_MODER0 << (BSP_CHECK_PIN_7 * 2));
		BSP_CHECK_PORT->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (BSP_CHECK_PIN_7 * 2));
		
		BSP_CHECK_PORT->ODR   &= ~((MASK_BIT_6) | (MASK_BIT_7));
		
		temp_u32 = BSP_CHECK_PORT->IDR;
		
		if(temp_u32 & (MASK_BIT_6))
		{
			/* Pin 6 is high only for Watchdog */
			gIF.BSP_Type = BSP_ExtWatchdog;
		}else
		{
			if(temp_u32 & (MASK_BIT_7))
			{
				/* Pin 7 is high only for Pilot */
				gIF.BSP_Type = BSP_Pilot;
			}else
			{
				/* This shall be a Torque sensor */
				gIF.BSP_Type = BSP_TorqueSensor;
			}
		}
	}
#else
		gIF.BSP_Type = BSP_Pilot;
#endif
	
	switch(gIF.BSP_Type)
	{
		case BSP_Pilot:
			gIF.pInit 	= &Usart1Init;
			gIF.pRecv 	= &Usart1Recv;
			gIF.pSend 	= &Usart1Send;
			gIF.pReset 	= &Usart1Reset;
			gIF.pDeInit = &Usart1DeInit;
			break;
		
		case BSP_TorqueSensor:
			gIF.pInit 	= &Usart1Init;
			gIF.pRecv 	= &Usart1Recv;
			gIF.pSend 	= &Usart1Send;
			gIF.pReset 	= &Usart1Reset;
			gIF.pDeInit = &Usart1DeInit;	
			break;
		
		case BSP_ExtWatchdog:
			gIF.pInit 	= NULL;
			gIF.pRecv 	= NULL;
			gIF.pSend 	= NULL;
			gIF.pDeInit = &Usart1DeInit;
			break;
		
		case BSP_NucleoF0x:
			gIF.pInit 	= &Usart1Init;
			gIF.pRecv 	= &Usart1Recv;
			gIF.pSend 	= &Usart1Send;
			gIF.pReset 	= &Usart1Reset;
			gIF.pDeInit = &Usart1DeInit;
			break;
		
		case BSP_STM32F042:
		default:
			gIF.pInit = NULL;
			break;
	}
	
	gIF.pInit(gIF.BSP_Type);
	FlashInit(gIF.BSP_Type);
	
	return(&gIF);
}

void BSP_DeInit(void)
{
	
}

