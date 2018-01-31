/******************************************************************************/
/**
* @file main.c
* @brief Main calling sequence for bootloader.
*
******************************************************************************/
/* ***************** Header / include files ( #include ) **********************/
#if(CPU_TYPE == STM32F031K6)
    #include "Usart1.h"
    #include "Spi1.h"
#else
    #include "Can.h"
#endif
#include "Protocol.h"
#include "Timer.h"
#include "Flash.h"
#include "Packet.h"
#include "Gpio.h"
#include "CRC.h"
#include "ProtoStateMachine.h"
#include "LED.h"
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
		LED_Init();
    Usart1Init();
    Spi1Init();
    PacketInit();
    FlashInit();
    //TimerInit();
    //TimerStartTimeout();
		LED_Off();
    for(;;)
    {
			ProtocolSM_Run();
    }
}
