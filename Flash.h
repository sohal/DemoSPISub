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
#define FLASH_PROGRAM_START_ADDRESS 0x08001000
#define NUM_OF_FLASH_PAGES          28
/* ********************* Type definitions ( typedef ) *************************/
/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/
/*******************************************************************************/
void FlashInit(void);
uint8_t FlashWrite(uint8_t* buf, uint16_t size);
uint8_t FlashVerify(uint8_t* buf, uint16_t size);
void FlashErase(void);
uint8_t FlashIsErased(void);
void FlashLock(void);
#endif

/* end of Flash.h */
