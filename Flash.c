/******************************************************************************/
/**
* @file Flash.c
* @brief Implement Flash driver
*
*******************************************************************************/
/* ***************** Header / include files ( #include ) **********************/
#include "stm32f0xx.h"
#include "Flash.h"
#include "Gpio.h"
/* *************** Constant / macro definitions ( #define ) *******************/
#define PAGE_SIZE_BYTES 0x400

/* ********************* Type definitions ( typedef ) *************************/

/* *********************** Global data definitions ****************************/
/* **************** Global constant definitions ( const ) *********************/
/* ***************** Modul global data segment ( static ) *********************/
static uint8_t IsErased;
static volatile uint16_t *ar;
static volatile uint16_t *vr;

/******************************************************************************/
/**
* void FlashInit(void)
* @brief Initialise the Flash and unlock it.
*
*******************************************************************************/
void FlashInit(void)
{
    IsErased = 0;
    ar = (uint16_t *)FLASH_PROGRAM_START_ADDRESS;
    vr = (uint16_t *)FLASH_PROGRAM_START_ADDRESS;
    // Unlock Flash
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
    while((FLASH->CR & FLASH_CR_LOCK) != 0);
    //FlashErase();
}

/******************************************************************************/
/**
* uint8_t FlashWrite(uint8_t* buf, uint16_t size)
* @brief Write to Flash and lock it afterwards.
*
* @param[in] buf pointer to data to be written to flash
* @param[in] size number of bytes
* @returns   1 if successful
*            or
*            0 if an error occurs.
*
*******************************************************************************/
uint8_t FlashWrite(uint8_t* buf, uint16_t size)
{
    uint16_t i = 0;
    // Program Flash Page
    FLASH->SR |= FLASH_SR_PGERR | FLASH_SR_WRPRTERR;
    while(i < size) 
    {
        FLASH->CR |= FLASH_CR_PG;
        *ar++ = (uint16_t)(buf[i+1] << 8) | buf[i];
        while((FLASH->SR & FLASH_SR_BSY) != 0);
        FLASH->CR &= ~FLASH_CR_PG;
        if((FLASH->SR & (FLASH_SR_PGERR | FLASH_SR_WRPRTERR)) != 0) 
        {
            FLASH->SR |= FLASH_SR_PGERR | FLASH_SR_WRPRTERR;
            for(;;);
        }
        i += 2;
    }
    return 1; // only 1024 bytes, need to check overflow etc.
}

/******************************************************************************/
/**
* uint8_t FlashVerify(uint8_t* buf, uint16_t size)
* @brief Verify code in Flash.
*
* @param[in] buf pointer to data to be compared with flash
* @param[in] size number of bytes
* @returns   1 if matches
*            or
*            0 if doesn't match.
*
*******************************************************************************/
uint8_t FlashVerify(uint8_t* buf, uint16_t size)
{
    uint16_t i = 0;
    while(i < size) 
    {
        if(*vr++ != ((uint16_t)(buf[i+1] << 8) | buf[i]))
        {
            return 0;
        }
        i += 2;
    }
    return 1;
}

/******************************************************************************/
/**
* void FlashErase(void)
* @brief Erase Flash from 0x08001000 to 0x08008000 by page, each page is 1K.
*
*******************************************************************************/
void FlashErase(void)
{
#if 0
    // Erase 1 page
    FLASH->SR |= FLASH_SR_PGERR | FLASH_SR_WRPRTERR;
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = (uint32_t)FLASH_PROGRAM_START_ADDRESS;
    FLASH->CR |= FLASH_CR_STRT;
    while((FLASH->SR & FLASH_SR_BSY) != 0);
    FLASH->CR &= ~FLASH_CR_PER;
    if((FLASH->SR & (FLASH_SR_PGERR | FLASH_SR_WRPRTERR)) != 0)
    {
        FLASH->SR |= FLASH_SR_PGERR | FLASH_SR_WRPRTERR;
        for(;;);
    }
    ar = (uint16_t *)FLASH_PROGRAM_START_ADDRESS;
    vr = (uint16_t *)FLASH_PROGRAM_START_ADDRESS;
    IsErased = 1;
#endif
    uint32_t flashAdr = (uint32_t)FLASH_PROGRAM_START_ADDRESS;
    GpioSet();
    for(uint8_t i = 0; i < NUM_OF_FLASH_PAGES; i++)
    {
        FLASH->SR |= FLASH_SR_PGERR | FLASH_SR_WRPRTERR;
        FLASH->CR |= FLASH_CR_PER;
        FLASH->AR = flashAdr;
        FLASH->CR |= FLASH_CR_STRT;
        while((FLASH->SR & FLASH_SR_BSY) != 0);
        FLASH->CR &= ~FLASH_CR_PER;
        if((FLASH->SR & (FLASH_SR_PGERR | FLASH_SR_WRPRTERR)) != 0)
        {
            FLASH->SR |= FLASH_SR_PGERR | FLASH_SR_WRPRTERR;
            for(;;);
        }
        flashAdr = flashAdr + PAGE_SIZE_BYTES;
    }
    GpioReset();
    ar = (uint16_t *)FLASH_PROGRAM_START_ADDRESS;
    vr = (uint16_t *)FLASH_PROGRAM_START_ADDRESS;
    IsErased = 1;
}

/******************************************************************************/
/**
* uint8_t FlashIsErased(void)
* @brief Check if flash has been erased.
*
*******************************************************************************/
uint8_t FlashIsErased(void)
{
    return IsErased;
}

/******************************************************************************/
/**
* void FlashLock(void)
* @brief Lock flash.
*
*******************************************************************************/
void FlashLock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}
