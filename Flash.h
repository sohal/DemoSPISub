/******************************************************************************/
/**
* @file Flash.h
* @brief Implement Flash driver
*
*******************************************************************************/
#ifndef FLASH_H
#define FLASH_H

/* ***************** Header / include files ( #include ) **********************/
#include <stdint.h>
#include "CRC.h"
#include "Common.h"
#include "BSP.h"

/* *************** Constant / macro definitions ( #define ) *******************/

/* ********************* Type definitions ( typedef ) *************************/
/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/
/*******************************************************************************/
void FlashInit(tBSPType BSPType);
uint8_t FlashWrite(uint8_t* buf, uint16_t size);
uint8_t FlashVerify(uint8_t* buf, uint16_t size);
uint8_t FlashErase(void);
void FlashLock(void);
uint8_t FlashWriteFWParam(tFIRMWARE_PARAM fwParam);
uint8_t FlashVerifyFWParam(tFIRMWARE_PARAM fwParam);
uint8_t FlashVerifyFirmware(void);

#endif

/* end of Flash.h */
