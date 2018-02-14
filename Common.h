/******************************************************************************/
/**
* @file Common.h
* @brief Definition and type defines of the common stuff
*
*******************************************************************************/
#ifndef COMMON_H
#define COMMON_H
/* ***************** Header / include files ( #include ) **********************/
/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/**
* @enum eFUNCTION_RETURN
* @brief Return value of the transmit functions.
*/
typedef enum
{
    eFunction_Ok       = 0, /**< Everything is fine */
    eFunction_Timeout  = 1, /**< Polling timeout    */
    eFunction_Error    = 2  /**< Data lost          */
}eFUNCTION_RETURN;

#ifndef NULL
#define NULL 							((void *)0)
#endif

#define MASK_BIT_0				(1UL <<  0U)
#define MASK_BIT_1				(1UL <<  1U)
#define MASK_BIT_2				(1UL <<  2U)
#define MASK_BIT_3				(1UL <<  3U)
#define MASK_BIT_4				(1UL <<  4U)
#define MASK_BIT_5				(1UL <<  5U)
#define MASK_BIT_6				(1UL <<  6U)
#define MASK_BIT_7				(1UL <<  7U)
#define MASK_BIT_8				(1UL <<  8U)
#define MASK_BIT_9				(1UL <<  9U)
#define MASK_BIT_10				(1UL <<  10U)
#define MASK_BIT_11				(1UL <<  11U)
#define MASK_BIT_12				(1UL <<  12U)
#define MASK_BIT_13				(1UL <<  13U)
#define MASK_BIT_14				(1UL <<  14U)
#define MASK_BIT_15				(1UL <<  15U)

typedef enum bsptype {
	BSP_UnKnown,
	BSP_Pilot,
	BSP_TorqueSensor,
	BSP_ExtWatchdog,
	BSP_STM32F042,
	BSP_NucleoF0x
}tBSPType;

typedef struct  {
	tBSPType BSP_Type;
	void (*pInit)(const tBSPType);
	void (*pSend)(uint8_t *, uint16_t);
	eFUNCTION_RETURN (*pRecv)(uint8_t *, uint16_t);
	void (*pReset)(void);
	void (*pDeInit)(void);
	uint32_t BootTimeoutTicks;
	uint32_t AppStartTicks;
	uint32_t CommDoneTicks;
	uint32_t TwoBytesTicks;
}tBSPStruct;


/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/

#endif

/* end of Packet.h */
