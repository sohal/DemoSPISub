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
#include "Gpio.h"
#include "BSP.h"

#define CAN_MAX_DATA_LENGTH		(8U)  // Maximum number of bytes in CAN bus (4H + 4L)
#define min(a,b) 							((a)<(b)?(a):(b))
/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/
typedef union myCanData{
	uint32_t 	uint4Bytes;
	uint8_t 	uint1Byte[4];
}tCANDataUnion;

void CanInit(tBSPType BSPType);
void CanSend(uint8_t *pTxData, uint16_t size);
void CanReset(void);
eFUNCTION_RETURN CanRecv(uint8_t *pRxData, uint16_t size);
void CanDeInit(void);
#endif
