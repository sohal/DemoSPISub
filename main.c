/******************************************************************************/
/**
* @file main.c
* @brief Main calling sequence for bootloader.
*
******************************************************************************/
/* ***************** Header / include files ( #include ) **********************/
#include "Usart1.h"
#include "Protocol.h"
#include "Timer.h"
#include "Flash.h"
#include "Packet.h"
#include "Gpio.h"

/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/* *********************** Global data definitions ****************************/
/* **************** Global constant definitions ( const ) *********************/
/* ***************** Modul global data segment ( static ) *********************/
/* *************** Modul global constants ( static const ) ********************/
/* **************** Local func/proc prototypes ( static ) *********************/

/******************************************************************************/

int main (void)
{
    GpioInit();
    Usart1Init();
    PacketInit();
    ProtocolInit();
    FlashInit();
    TimerInit();
    TimerStartTimeout();
    for(;;)
    {
        ProtocolStateProcess();
    }
}
