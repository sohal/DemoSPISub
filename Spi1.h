/******************************************************************************/
/**
* @file Spi1.h
* @brief Implement SPI1
*
*******************************************************************************/
#ifndef SPI1_H
#define SPI1_H

/* ***************** Header / include files ( #include ) **********************/
#include "stm32f0xx.h"
#include "Common.h"

/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/
void Spi1Init(void);
eFUNCTION_RETURN Spi1Transmit(uint8_t *pTxData, uint16_t size);
eFUNCTION_RETURN Spi1Receive(uint8_t *pRxData, uint16_t size);
uint8_t Spi1ByteReceived(void);

#endif
