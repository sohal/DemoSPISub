#include "LED.h"

void LED_Init(void)
{
	 RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	  
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (GPIO_PIN_1 * 2));
    GPIOA->OSPEEDR |= ((uint32_t)GPIO_Speed_Level_3 << (GPIO_PIN_1 * 2));
    GPIOA->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)GPIO_PIN_1));
    GPIOA->OTYPER |= (uint16_t)(((uint16_t)GPIO_OType_PP) << ((uint16_t)GPIO_PIN_1));
    GPIOA->MODER &= ~(GPIO_MODER_MODER0 << (GPIO_PIN_1 * 2));
    GPIOA->MODER |= ((uint32_t)GPIO_Mode_OUT << (GPIO_PIN_1 * 2));
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (GPIO_PIN_1 * 2));
    GPIOA->PUPDR |= ((uint32_t)GPIO_PuPd_NOPULL << (GPIO_PIN_1 * 2));
}



/******************************************************************************/
/**
* void LED_Off(void)
* @brief Set Gpio B3 to high.
*
*******************************************************************************/
void LED_Off(void)
{
    GPIOA->BSRR = (uint16_t)(((uint16_t)1) << ((uint16_t)GPIO_PIN_1));
}

/******************************************************************************/
/**
* void LED_On(void)
* @brief Set Gpio B3 to low.
*
*******************************************************************************/
void LED_On(void)
{
    GPIOA->BRR = (uint16_t)(((uint16_t)1) << ((uint16_t)GPIO_PIN_1));
}
