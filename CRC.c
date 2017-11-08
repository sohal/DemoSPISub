/******************************************************************************/
/**
* @file CRC.c
* @brief CRC16 checksum calculator
*
*******************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include "CRC.h"

#define CRC16 0xAA55

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
    uint16_t out = 0;
    uint8_t bits_read = 0, bit_flag;
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
    return out;
}
