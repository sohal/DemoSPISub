/******************************************************************************/
/**
* @file Usart1.c
* @brief Implement usart1
*
*******************************************************************************/

/* ***************** Header / include files ( #include ) **********************/
#include "Usart1.h"
#include "Gpio.h"
#include "Timeout.h"
/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/* *********************** Global data definitions ****************************/
/* **************** Global constant definitions ( const ) *********************/
/* ***************** Modul global data segment ( static ) *********************/
static uint16_t index =0;
/* *************** Modul global constants ( static const ) ********************/
/* **************** Local func/proc prototypes ( static ) *********************/

/******************************************************************************/
/**
* void Usart1Init(void)
* @brief Configure USART1 (STM32F031:PA2(TX),PA15(RX)) and initialze variables.
*
*******************************************************************************/
void Usart1Init(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIOA->AFR[GPIO_PIN_2 >> 3] &= ~((uint32_t)0xF << ((uint32_t)(((uint32_t)GPIO_PIN_2) & (uint32_t)0x07) * 4));
    GPIOA->AFR[GPIO_PIN_2 >> 3] |= ((uint32_t)(GPIO_AF_1) << ((uint32_t)(((uint32_t)GPIO_PIN_2) & (uint32_t)0x07) * 4));
    GPIOA->AFR[GPIO_PIN_15 >> 3] &= ~((uint32_t)0xF << ((uint32_t)(((uint32_t)GPIO_PIN_15) & (uint32_t)0x07) * 4));
    GPIOA->AFR[GPIO_PIN_15 >> 3] |= ((uint32_t)(GPIO_AF_1) << ((uint32_t)(((uint32_t)GPIO_PIN_15) & (uint32_t)0x07) * 4));

    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (GPIO_PIN_2 * 2));
    GPIOA->OSPEEDR |= ((uint32_t)GPIO_Speed_Level_3 << (GPIO_PIN_2 * 2));
    GPIOA->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)GPIO_PIN_2));
    GPIOA->OTYPER |= (uint16_t)(((uint16_t)GPIO_OType_PP) << ((uint16_t)GPIO_PIN_2));
    GPIOA->MODER &= ~(GPIO_MODER_MODER0 << (GPIO_PIN_2 * 2));
    GPIOA->MODER |= ((uint32_t)GPIO_Mode_AF << (GPIO_PIN_2 * 2));
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (GPIO_PIN_2 * 2));
    GPIOA->PUPDR |= ((uint32_t)GPIO_PuPd_UP << (GPIO_PIN_2 * 2));

    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (GPIO_PIN_15 * 2));
    GPIOA->OSPEEDR |= ((uint32_t)GPIO_Speed_Level_3 << (GPIO_PIN_15 * 2));
    GPIOA->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)GPIO_PIN_15));
    GPIOA->OTYPER |= (uint16_t)(((uint16_t)GPIO_OType_PP) << ((uint16_t)GPIO_PIN_15));
    GPIOA->MODER &= ~(GPIO_MODER_MODER0 << (GPIO_PIN_15 * 2));
    GPIOA->MODER |= ((uint32_t)GPIO_Mode_AF << (GPIO_PIN_15 * 2));
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (GPIO_PIN_15 * 2));
    GPIOA->PUPDR |= ((uint32_t)GPIO_PuPd_UP << (GPIO_PIN_15 * 2));

    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    //  RCC->CFGR3 &= ~3;
    //  RCC->CFGR3 |= 0;  // USART1 Clock Source = PCLK

    //USART1->BRR = __USART_BRR(8000000UL, 9600);  // 8MHz, 9600 baud
    //USART1->BRR = __USART_BRR(8000000UL, 115200);  // 8MHz, 115200 baud
    USART1->BRR = __USART_BRR(8000000UL, 460800);  // 8MHz, 460800 baud
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;  // 8N1
}

/******************************************************************************/
/**
* void Usart1Transmit(uint8_t *pTxData, uint16_t size)
* @brief Implement usart1 send.
*
* @param[in] pTxData pointer to the data to be transmitted
* @param[in] size number of bytes
*
*******************************************************************************/
void Usart1Transmit(uint8_t *pTxData, uint16_t size)
{
    while(size > 0)
    {
        size--;
        while((USART1->ISR & USART_ISR_TXE) == 0);
        USART1->TDR = pTxData[size];
    }
}

/******************************************************************************/
/**
* eFUNCTION_RETURN Usart1Receive(uint8_t *pRxData, uint16_t size)
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
eFUNCTION_RETURN Usart1Receive(uint8_t *pRxData, const uint16_t size)
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
* void Usart1ReceiveReady(void)
*
* @brief Reset receive pointer index
*
* @returns    none
*
*******************************************************************************/
void Usart1ReceiveReady(void)
{
	index = 0;
}


/******************************************************************************/
/**
* uint8_t Usart1ByteReceived(void)
*
* @brief Check if there is any data on bus. Read the first byte out.
*        Fetch a byte takes ~2 us with 9600 baudrate.
*
* @returns    1 if there is a byte on bus.
*             or
*             0 if there is nothing.
*
*******************************************************************************/
uint8_t Usart1ByteReceived(void)
{
    if(USART1->ISR & USART_ISR_RXNE)
    {
        //firstByte = USART1->RDR;
        //firstByteRead = 1;
        return 1;
    }
    else
    {
        return 0;
    }
}
