/******************************************************************************/
/**
* @file Can.c
* @brief Implement Can
*
*******************************************************************************/

/* ***************** Header / include files ( #include ) **********************/
#include "Can.h"
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
    #if 0
    RCC->APB1ENR |= RCC_APB1ENR_CANEN;
  
    // Enter init mode
  
    CAN->MCR = CAN_MCR_INRQ;
    while((CAN->MSR & CAN_MSR_INAK) == 0);
  
    CAN->MCR |= CAN_MCR_NART;
    CAN->BTR = (2 << 20) | (3 << 16) | (5 << 0);
  
#define CAN_ID  33
  
    CAN->FMR |= CAN_FMR_FINIT;
    CAN->FA1R &= ~CAN_FA1R_FACT0;  // deactivate filter
    CAN->FS1R |= CAN_FS1R_FSC0;  // set 32-bit scale configuration
    CAN->FM1R |= CAN_FM1R_FBM0;  // set two 32-bit identifier list mode
    CAN->sFilterRegister[0].FR1 = (CAN_ID << 21);  // standard ID
    CAN->sFilterRegister[0].FR2 = (CAN_ID << 21);
//    CAN->sFilterRegister[0].FR1 = (CAN_ID << 3) | (1 << 2);  // extended ID
//    CAN->sFilterRegister[0].FR2 = (CAN_ID << 3) | (1 << 2);
    CAN->FFA1R &= ~CAN_FFA1R_FFA0;  // assign filter to FIFO 0
    CAN->FA1R |= CAN_FA1R_FACT0;  // activate filter
    CAN->FMR &= ~CAN_FMR_FINIT;+
		#endif
}

