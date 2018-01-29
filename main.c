/******************************************************************************/
/**
* @file main.c
* @brief Main calling sequence for bootloader.
*
******************************************************************************/
/* ***************** Header / include files ( #include ) **********************/
#if defined(STM32F031x6)
    #include "Usart1.h"
    #include "Spi1.h"
#elif defined(STM32F042x6)
    #include "Can.h"
#else 
    #error "CPU type needs to be specified based on the device, either STMF031K6 or STM32F042K6"
#endif
#include "Protocol.h"
#include "Timer.h"
#include "Flash.h"
#include "Packet.h"
#include "Gpio.h"
#include "CRC.h"

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
#if defined(STM32F031x6)
    Usart1Init();
    Spi1Init();
#elif defined(STM32F042x6)
    CanInit();
#endif
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
