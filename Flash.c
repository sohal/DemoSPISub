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
/* ********************* Type definitions ( typedef ) *************************/
/* *********************** Global data definitions ****************************/
/* **************** Global constant definitions ( const ) *********************/
/* ***************** Modul global data segment ( static ) *********************/
static uint8_t isErased = 0U;
static volatile uint16_t *ar = NULL;
static volatile uint16_t *vr = NULL;
static uint32_t APP_CRC_ADDRESS = 0UL;
static uint32_t APP_SIZE_ADDRESS = 0UL;
static uint32_t APP_TOTAL_FLASH_PAGES = 0UL;
/******************************************************************************/
/**
* void FlashInit(void)
* @brief Initialise the Flash and unlock it.
*
*******************************************************************************/
void FlashInit(tBSPType BSPType)
{
    isErased = 0;
    ar = (uint16_t *)BSP_APPLICATION_START;
    vr = (uint16_t *)BSP_APPLICATION_START;
    // Unlock Flash
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
    while((FLASH->CR & FLASH_CR_LOCK) != 0);
    if(BSPType == BSP_Pilot)
		{
			APP_CRC_ADDRESS = 	BSP_PILOT_FLASH_END - 3UL;
			APP_SIZE_ADDRESS =	BSP_PILOT_FLASH_END - 1UL;
			APP_TOTAL_FLASH_PAGES = BSP_PILOT_PROGRAM_SECTORS;
		}else
		{
			APP_CRC_ADDRESS = 	BSP_TORQUE_FLASH_END - 3UL;
			APP_SIZE_ADDRESS =	BSP_TORQUE_FLASH_END - 1UL;
			APP_TOTAL_FLASH_PAGES = BSP_TORQUE_PROGRAM_SECTORS;
		}
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
        i += sizeof(uint16_t);
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
* uint8_t FlashErase(void)
* @brief Erase Flash from 0x08001000 to 0x08008000 by page, each page is 1K.
*
* @returns   1 if successful
*            or
*            0 if an error occurs.
*
*******************************************************************************/
uint8_t FlashErase(void)
{
    uint32_t flashAdr = (uint32_t)BSP_APPLICATION_START;
    for(uint8_t i = 0; i < APP_TOTAL_FLASH_PAGES; i++)
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
            return 0;
        }
        flashAdr = flashAdr + BSP_PAGE_SIZE_BYTES;
    }
    ar = (uint16_t *)BSP_APPLICATION_START;
    vr = (uint16_t *)BSP_APPLICATION_START;
    isErased = 1;
    return 1;
}

/******************************************************************************/
/**
* uint8_t FlashIsErased(void)
* @brief Check if flash has been erased.
*
*******************************************************************************/
uint8_t FlashIsErased(void)
{
    return isErased;
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

/******************************************************************************/
/**
* uint8_t FlashWriteFWParam(tFIRMWARE_PARAM fwParam)
* @brief Write 4 bytes firmware parameters (i.e. FW crc and length) to a fixed 
*        Flash address.
*
* @param[in] fwParam firmware parameters to be written to flash
* @returns   1 if successful
*            or
*            0 if an error occurs.
*
*******************************************************************************/
uint8_t FlashWriteFWParam(tFIRMWARE_PARAM fwParam)
{
    uint16_t *ad = (uint16_t *)APP_CRC_ADDRESS;
    FLASH->SR |= FLASH_SR_PGERR | FLASH_SR_WRPRTERR;
    /* Write FW CRC */
    FLASH->CR |= FLASH_CR_PG;
    *ad = (uint16_t)fwParam.u16FWCRC;
    while((FLASH->SR & FLASH_SR_BSY) != 0);
    FLASH->CR &= ~FLASH_CR_PG;
    if((FLASH->SR & (FLASH_SR_PGERR | FLASH_SR_WRPRTERR)) != 0)
    {
        FLASH->SR |= FLASH_SR_PGERR | FLASH_SR_WRPRTERR;
        return 0;
    }
    ad = (uint16_t *)APP_SIZE_ADDRESS;
    /* Write FW length */
    FLASH->CR |= FLASH_CR_PG;
    *ad = (uint16_t)fwParam.u16FWLen;
    while((FLASH->SR & FLASH_SR_BSY) != 0);
    FLASH->CR &= ~FLASH_CR_PG;
    if((FLASH->SR & (FLASH_SR_PGERR | FLASH_SR_WRPRTERR)) != 0)
    {
        FLASH->SR |= FLASH_SR_PGERR | FLASH_SR_WRPRTERR;
        return 0;
    }
    return 1;
}

/******************************************************************************/
/**
* uint8_t FlashVerifyFWParam(tFIRMWARE_PARAM fwParam)
* @brief Verify firmware parameters in Flash after the write.
*
* @param[in] fwParam firmware parameters to be compared with flash
* @returns   1 if matches
*            or
*            0 if doesn't match.
*
*******************************************************************************/
uint8_t FlashVerifyFWParam(tFIRMWARE_PARAM fwParam)
{
    uint16_t *ad = (uint16_t *)APP_CRC_ADDRESS;
    if(*ad != fwParam.u16FWCRC)
    {
        return 0;
    }
    ad = (uint16_t *)APP_SIZE_ADDRESS;
    if(*ad != fwParam.u16FWLen)
    {
        return 0;
    }
    return 1;
}

/******************************************************************************/
/**
* uint8_t FlashVerifyFirmware(void)
* @brief Verify firmware in Flash by comparing the stored crc with the 
*        calculated crc.
*
* @returns   1 if matches
*            or
*            0 if doesn't match.
*
*******************************************************************************/
uint8_t FlashVerifyFirmware(void)
{
	volatile uint16_t i = 0;
	uint32_t temp32 = *(uint32_t *)APP_CRC_ADDRESS;
	volatile const uint16_t lenFromHost = (uint16_t)(temp32 >> 16U);
	volatile const uint16_t crcFromHost = (uint16_t)(temp32 & 0x0000FFFFUL);
	uint16_t dataByte = 0;
	uint16_t CRCtemp = 0;
	uint16_t *fwar = (uint16_t*)BSP_ALLBOARD_BOOT_END;
	/* Read from FLASH_CRC_LENGTH_ADDRESS the firmware crc and length from host */

	/** Check if the length is within flash range or the read flash will fail */
	if(lenFromHost > (APP_CRC_ADDRESS - BSP_APPLICATION_START))
	{
		return 0;
	}

	/* Calculate local crc */
	while(i < lenFromHost) 
	{
		/* Read from address of the firmware and calculate crc */
		dataByte = *fwar++;
		CRCtemp = CRCCalc16((uint8_t *)&dataByte, 2, CRCtemp);
		i += 2;
	}
	if(CRCtemp == crcFromHost)
	{
		return 1;
	}
	return 0;
}
