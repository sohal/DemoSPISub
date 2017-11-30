/******************************************************************************/
/**
* @file CRC.h
* @brief CRC16 checksum calculator
*
*******************************************************************************/
#ifndef CRC_H
#define CRC_H

/* ***************** Header / include files ( #include ) **********************/
#include "Packet.h"

/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/
void CRCInit(void);
uint16_t CRCCalc16(const uint8_t *data, uint16_t size);
tFIRMWARE_PARAM CRCGetFWParam(void);

#endif

/* end of CRC.h */
