/******************************************************************************/
/**
* @file Timer.h
* @brief Set up timer and timer interrupt
*
*******************************************************************************/
#ifndef TIMER_H
#define TIMER_H

/* ***************** Header / include files ( #include ) **********************/
#include <stdint.h>

/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/
void TimerInit(void);
void TimerDeInit(void);
void TimerStartTimeout(void);
uint8_t TimerIsTimeout(uint32_t timeout);

#endif

/* end of Timer.h */
