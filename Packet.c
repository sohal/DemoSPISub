/******************************************************************************/
/**
* @file Packet.c
* @brief Process the packet
*
*******************************************************************************/

/* ***************** Header / include files ( #include ) **********************/
#include <stdint.h>
#include <stddef.h>
#include "Packet.h"
#include "CRC.h"

/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/* *********************** Global data definitions ****************************/
/* **************** Global constant definitions ( const ) *********************/
/* ***************** Modul global data segment ( static ) *********************/
static uint16_t expectedSN;

/* *************** Modul global constants ( static const ) ********************/
/* **************** Local func/proc prototypes ( static ) *********************/

/******************************************************************************/
/**
* void PacketInit(void)
* @brief Initialize function.
*******************************************************************************/
void PacketInit(void)
{
    expectedSN = 0;
}

/******************************************************************************/
/**
* ePACKET_STATUS PacketProcess(tDATA_PACKET packet)
* @brief Process the 68 bytes data packet.
* @param[in] packet 68 bytes data packet
* @returns   ePACKET_Ok if successful
*            or
*            ePACKET_CRCError if CRC check fails.
*            or
*            ePACKET_SNError if serial number is wrong.
*******************************************************************************/
ePACKET_STATUS PacketProcess(tDATA_PACKET packet)
{
    ePACKET_STATUS errStatus = ePACKET_Ok;
    if(CRCCalc16(&packet.u8Data[0], BLOCK_SIZE, 0) != packet.u16CRC)
    {
        errStatus = ePACKET_CRCError;
    }
    else if(packet.u16SeqCnt != expectedSN)
    {
        errStatus = ePACKET_SNError;
    }
    else
    {
        // CRC and sequence number OK
        expectedSN = packet.u16SeqCnt + 1;
    }
    return errStatus;
}
