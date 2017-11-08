/******************************************************************************/
/**
* @file Protocol.h
* @brief Implementation of the protocol layer
*
*******************************************************************************/
#ifndef PROTOCOL_H
#define PROTOCOL_H
/* ***************** Header / include files ( #include ) **********************/
#include "Command.h"
/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/**
* @enum eSTATES
* @brief Bootloader states
*/
typedef enum
{
    eSTATE_WaitForCmd     = 0,/**< Wait for "ready" command from the bootloader */
    eSTATE_BootloaderMode = 1,
    //eSTATE_EraseFlash     = 2,
    eSTATE_WaitForPacket  = 2,
    eSTATE_OKtoSender     = 3,
    eSTATE_WritePacket    = 4,
    eSTATE_ReceivePacket  = 5,
    eSTATE_Error          = 6,
    eSTATE_ExitBootloader = 7
}eSTATES;

/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/
void ProtocolInit(void);
void ProtocolStateProcess(void);

#endif

/* end of Protocol.h */
