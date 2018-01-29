/******************************************************************************/
/**
* @file Gpio.c
* @brief Setup GPIO B pins
*
*******************************************************************************/

/* ***************** Header / include files ( #include ) **********************/
#include "stm32f0xx.h"
#include "Gpio.h"

/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/* *********************** Global data definitions ****************************/
/* **************** Global constant definitions ( const ) *********************/
/* ***************** Modul global data segment ( static ) *********************/
/* *************** Modul global constants ( static const ) ********************/
/* **************** Local func/proc prototypes ( static ) *********************/

/******************************************************************************/
/**
* void GpioInit(void)
* @brief Confingure LD3 (STM32F031_NUCLEO:PB3)
*
*******************************************************************************/
void GpioInit(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (GPIO_PIN_3 * 2));
    GPIOB->OSPEEDR |= ((uint32_t)GPIO_Speed_Level_3 << (GPIO_PIN_3 * 2));
    GPIOB->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)GPIO_PIN_3));
    GPIOB->OTYPER |= (uint16_t)(((uint16_t)GPIO_OType_PP) << ((uint16_t)GPIO_PIN_3));
    GPIOB->MODER &= ~(GPIO_MODER_MODER0 << (GPIO_PIN_3 * 2));
    GPIOB->MODER |= ((uint32_t)GPIO_Mode_OUT << (GPIO_PIN_3 * 2));
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (GPIO_PIN_3 * 2));
    GPIOB->PUPDR |= ((uint32_t)GPIO_PuPd_NOPULL << (GPIO_PIN_3 * 2));
}

/******************************************************************************/
/**
* void GpioSet(void)
* @brief Set Gpio B3 to high.
*
*******************************************************************************/
void GpioB3Set(void)
{
    GPIOB->BSRR = (uint16_t)(((uint16_t)1) << ((uint16_t)GPIO_PIN_3));
}

/******************************************************************************/
/**
* void GpioReset(void)
* @brief Set Gpio B3 to low.
*
*******************************************************************************/
void GpioB3Reset(void)
{
    GPIOB->BRR = (uint16_t)(((uint16_t)1) << ((uint16_t)GPIO_PIN_3));
}

/******************************************************************************/
/**
* eCOMMUNICATION_INTERFACE GpioGetComIF(void)
* @brief Read in the communcation interface setting from Gpio.
* @returns via SPI or via USART
*
*******************************************************************************/
eCOMMUNICATION_INTERFACE GpioGetComIF(void)
{
    return eUSART;
}

