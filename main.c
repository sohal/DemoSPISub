/******************************************************************************/
/**
* @file main.c
* @brief Main calling sequence for bootloader.
*
******************************************************************************/
/* ***************** Header / include files ( #include ) **********************/
#include "Usart1.h"
#include "Flash.h"
#include "Packet.h"
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
	  LED_Init();
    Usart1Init(BSP_RxPin, BSP_TxPin, BSP_Baud);
    PacketInit();
    FlashInit();
		LED_On();
    for(;;)
    {
			ProtocolSM_Run();
    }
}
