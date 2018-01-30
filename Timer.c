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
volatile uint32_t tick;

/* **************** Global constant definitions ( const ) *********************/
/* ***************** Modul global data segment ( static ) *********************/
static uint32_t tickStart;

/* *************** Modul global constants ( static const ) ********************/
/* **************** Local func/proc prototypes ( static ) *********************/

void TIM2_IRQHandler(void) 
{
    TIM2->SR &= ~TIM_SR_UIF;
    tick++;
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
    tickStart = 0;
    tick = 0;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->ARR = 8000000;
    NVIC->ISER[0] |= (1 << TIM2_IRQn);
    TIM2->CR1 |= TIM_CR1_DIR | TIM_CR1_CEN;
    TIM2->DIER |= TIM_DIER_UIE;
}

/******************************************************************************/
/**
* void TimerDeInit(void)
* @brief Disable timer IRQ.
*
*******************************************************************************/
void TimerDeInit(void)
{
    NVIC->ICER[0] |= (1 << TIM2_IRQn);
    TIM2->CR1 = 0;
}

/******************************************************************************/
/**
* void TimerStartTimeout(void)
* @brief Start the timeout counter.
*
*******************************************************************************/
void TimerStartTimeout(void)
{
    tickStart = tick;
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
    // (tick - tickStart) is overflow safe. e.g. (uint8_t)2-(uint8_t)254 = (uint8_t)4 
    if((tick - tickStart) >= timeout)
    {
        retVal = 1;
    }
    else
    {
        retVal = 0;
    }
    return 0;
}
