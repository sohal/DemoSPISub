/******************************************************************************/
/**
* @file Usart1.h
* @brief Implement usart1
*
*******************************************************************************/
#ifndef USART1_H
#define USART1_H

/* ***************** Header / include files ( #include ) **********************/
#include "stm32f0xx.h"
#include "Common.h"
#include "Gpio.h"
#include "BSP.h"
/* *************** Constant / macro definitions ( #define ) *******************/
#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))

/* ********************* Type definitions ( typedef ) *************************/
/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/
void Usart1Init(tBSPType BSPType);
void Usart1Send(uint8_t *pTxData, uint16_t size);
void Usart1Reset(void);
eFUNCTION_RETURN Usart1Recv(uint8_t *pRxData, uint16_t size);
#endif
