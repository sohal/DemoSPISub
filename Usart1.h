/******************************************************************************/
/**
* @file Usart1.h
* @brief Implement usart1
*
*******************************************************************************/
#ifndef USART1_H
#define USART1_H

#include "stm32f0xx.h"

/**
* @enum eFUNCTION_RETURN
* @brief Return value of the functions.
*/
typedef enum
{
    eFunction_Ok       = 0, /**< Everything is fine */
    eFunction_Timeout  = 1, /**< Polling timeout    */
    eFunction_Error    = 2  /**< Data lost          */
}eFUNCTION_RETURN;

#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))

/* Function defines */
void Usart1Init(void);
void Usart1Transmit(uint8_t *pTxData, uint16_t size);
eFUNCTION_RETURN Usart1Receive(uint8_t *pRxData, uint16_t size);
uint8_t Usart1ByteReceived(void);
#endif
