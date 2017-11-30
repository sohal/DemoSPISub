/******************************************************************************/
/**
* @file CRC.c
* @brief CRC16 checksum calculator
*
*******************************************************************************/
/* ***************** Header / include files ( #include ) **********************/
#include <stdint.h>
#include <stddef.h>
#include "CRC.h"

/* *************** Constant / macro definitions ( #define ) *******************/
#define CRC16 0xAA55

/* ********************* Type definitions ( typedef ) *************************/
/* *********************** Global data definitions ****************************/
/* **************** Global constant definitions ( const ) *********************/
/* ***************** Modul global data segment ( static ) *********************/
static tFIRMWARE_PARAM fwParam;

/* **************** Local func/proc prototypes ( static ) *********************/

/******************************************************************************/
/**
* void CRCInit(void)
* @brief Initialize function.
*******************************************************************************/
void CRCInit(void)
{
    fwParam.u16FWCRC = 0;
    fwParam.u16FWLen = 0;
}

/******************************************************************************/
/**
* uint16_t CRCCalc16(const uint8_t *data, uint16_t size)
* @brief Generate 16 bits CRC for an input array.
*
* @param[in] data pointer to data array
* @param[in] size byte number of the array
* @returns   calculated 16 bits CRC.
*******************************************************************************/
uint16_t CRCCalc16(const uint8_t *data, uint16_t size)
{
    uint16_t out = fwParam.u16FWCRC;
    uint8_t bits_read = 0, bit_flag;
    fwParam.u16FWLen += size;
    /* Sanity check */
    if(data == NULL)
    {
        return 0;
    }
    while(size > 0)
    {
        bit_flag = out >> 15;
        /* Get next bit */
        out <<= 1;
        out |= (*data >> (7 - bits_read)) & 1;
        /* Increment bit counter */
        bits_read++;
        if(bits_read > 7)
        {
            bits_read = 0;
            data++;
            size--;
        }
        /* Cycle check */
        if(bit_flag)
        {
            out ^= CRC16;
        }
    }
    fwParam.u16FWCRC = out;
    return out;
}

/******************************************************************************/
/**
* uint16_t CRCGetFWParam(void)
* @brief Returns 16 bits firmware CRC and the firmware length.
*
*******************************************************************************/
tFIRMWARE_PARAM CRCGetFWParam(void)
{
    return fwParam;
}
