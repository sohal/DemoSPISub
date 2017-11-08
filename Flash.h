/******************************************************************************/
/**
* @file Flash.h
* @brief Implement Flash driver
*
*******************************************************************************/
/* ***************** Header / include files ( #include ) **********************/
#ifndef FLASH_H
#define FLASH_H
#include <stdint.h>
/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/
/*******************************************************************************/
void FlashInit(void);
uint8_t FlashWrite(uint8_t* buf);
uint8_t FlashVerify(uint8_t* buf);
void FlashErase(void);
uint8_t FlashIsErased(void);
void FlashLock(void);
#endif

/* end of Flash.h */
