#include "BSP.h"

static tBSPStruct gIF;
static void TorqueSensorCoreClockInit(void);
static void TorqueSensorCoreClockDeInit(void);

tBSPStruct* BSP_Init(void)
{
	gIF.pInit 						= NULL;
	gIF.pDeInit 					= NULL;
	gIF.pSend 						= NULL;
	gIF.pReset 						= NULL;
	gIF.BSP_Type 					= BSP_UnKnown;
	gIF.ThreeSecondsTicks	= TIMEOUT_3s;
	gIF.AppStartTicks		  = gIF.ThreeSecondsTicks - 4500UL;
	gIF.CommDoneTicks 		= gIF.AppStartTicks - 15500UL;
	gIF.TwoBytesTicks			= 3000UL;
	uint32_t temp_u32 		= 0UL;

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
	
#ifdef SELECT_TORQUE
	gIF.BSP_Type = BSP_TorqueSensor;
	#warning Torque Sensor is selected 
#else
	#ifdef SELECT_PILOT
	gIF.BSP_Type = BSP_Pilot;
	#warning Pilot is selected
	#else
	#error Select appropriate board
	#endif
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
			TorqueSensorCoreClockInit();
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
  
	/* Let's update the global SystemCoreClock variable just in case the system
	 * frequency has changed. Mandatory for calculations of delay for bootloader
	 * timeouts that are solely dependent on system ticks
	 */
	SystemCoreClockUpdate();
	/* Now calculate by what factor has the system changed it's core clock */
	temp_u32 = ( SystemCoreClock / BSP_ALLBOARD_HSI_FREQUENCY );
	
	gIF.AppStartTicks 		*= temp_u32;
	gIF.CommDoneTicks 		*= temp_u32;
	gIF.ThreeSecondsTicks *= temp_u32;
	gIF.TwoBytesTicks			*= temp_u32;
	
	gIF.pInit(gIF.BSP_Type);
	
	FlashInit(gIF.BSP_Type);
	
	return(&gIF);
}

void BSP_DeInit(void)
{
	if(gIF.BSP_Type == BSP_TorqueSensor)
	{
		TorqueSensorCoreClockDeInit();
	}
	//gif.pDeInit();
	
}

void TorqueSensorCoreClockInit(void)
{
    RCC->CR |= ((uint32_t)RCC_CR_HSION);                       /* Enable HSI */
    while ((RCC->CR & RCC_CR_HSIRDY) == 0);                  /* Wait for HSI Ready */

    RCC->CFGR = RCC_CFGR_SW_HSI;                             /* HSI is system clock */
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);  /* Wait for HSI used as system clock */

    FLASH->ACR  = FLASH_ACR_PRFTBE;                          /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_LATENCY;                         /* Flash 1 wait state */

    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;                         /* HCLK = SYSCLK */
    RCC->CFGR |= RCC_CFGR_PPRE_DIV1;                         /* PCLK = HCLK */

    RCC->CR &= ~RCC_CR_PLLON;                                /* Disable PLL */

    /*  PLL configuration:  = HSI/2 * 8 = 32 MHz */
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMUL);
    RCC->CFGR |=  (RCC_CFGR_PLLSRC_HSI_DIV2 | RCC_CFGR_PLLMUL16);  /* 16 for 64Mhz */

    RCC->CR |= RCC_CR_PLLON;                                 /* Enable PLL */
    while((RCC->CR & RCC_CR_PLLRDY) == 0) __NOP();           /* Wait till PLL is ready */

    RCC->CFGR &= ~RCC_CFGR_SW;                               /* Select PLL as system clock source */
    RCC->CFGR |=  RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  /* Wait till PLL is system clock src */
}

void TorqueSensorCoreClockDeInit(void)
{
	//ToDo
	//SystemClockUpdate();
}
