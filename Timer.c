/******************************************************************************/
/**
* @file Timer.c
* @brief Implementation timer and timer interrupt
*
*******************************************************************************/

/* ***************** Header / include files ( #include ) **********************/
#include "stm32f0xx.h"
#include "Timer.h"

/* *************** Constant / macro definitions ( #define ) *******************/

/* ********************* Type definitions ( typedef ) *************************/

/* *********************** Global data definitions ****************************/
volatile uint32_t Tick;
/* **************** Global constant definitions ( const ) *********************/

/* ***************** Modul global data segment ( static ) *********************/
static uint32_t TickStart;
/* *************** Modul global constants ( static const ) ********************/

/* **************** Local func/proc prototypes ( static ) *********************/

void TIM2_IRQHandler(void) 
{
    TIM2->SR &= ~TIM_SR_UIF;
    Tick++;
}

/******************************************************************************/
/**
* void TimerInit(void)
* @brief Initialise the timer.
*
*******************************************************************************/
void TimerInit(void)
{
    // Configure Timer
    TickStart = 0;
    Tick = 0;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->ARR = 8000000;
    NVIC->ISER[0] |= (1 << TIM2_IRQn);
    TIM2->CR1 |= TIM_CR1_DIR | TIM_CR1_CEN;
    TIM2->DIER |= TIM_DIER_UIE;
}

/******************************************************************************/
/**
* void TimerStartTimeout(void)
* @brief Start the timeout counter.
*
*******************************************************************************/
void TimerStartTimeout(void)
{
    TickStart = Tick;
}

/******************************************************************************/
/**
* uint8_t TimerIsTimeout(uint32_t timeout)
* @brief Check if timeout.
*
* @param[in] timeout timeout value in second
* @returns   1 if timeout
*            or
*            0 if no timeout.
*
*******************************************************************************/
uint8_t TimerIsTimeout(uint32_t timeout)
{
    uint8_t retVal = 0;
    // (Tick - TickStart) is overflow safe. e.g. (uint8_t)2-(uint8_t)254 = (uint8_t)4 
    if((Tick - TickStart) >= timeout)
    {
        retVal = 1;
    }
    else
    {
        retVal = 0;
    }
    return retVal;
}
