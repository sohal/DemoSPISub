/******************************************************************************/
/**
* @file Can.h
* @brief Implement CAN
*
*******************************************************************************/
#ifndef CAN_H
#define CAN_H

/* ***************** Header / include files ( #include ) **********************/
#include "stm32f0xx.h"
#include "Common.h"

/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/
void CanInit(void);
void CanTransmit(uint8_t *pTxData, uint16_t size);
eFUNCTION_RETURN CanReceive(uint8_t *pRxData, uint16_t size);
uint8_t CanByteReceived(void);

#endif
