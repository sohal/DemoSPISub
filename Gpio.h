/******************************************************************************/
/**
* @file Gpio.h
* @brief Setup GPIO pins for debugging
*
*******************************************************************************/
#ifndef GPIO_H
#define GPIO_H
/* ***************** Header / include files ( #include ) **********************/
/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
typedef enum 
{
    GPIO_Mode_IN  = 0x00,
    GPIO_Mode_OUT = 0x01,
    GPIO_Mode_AF  = 0x02,
    GPIO_Mode_AN  = 0x03
} GPIOMode_TypeDef;

typedef enum 
{
    GPIO_OType_PP = 0x00,
    GPIO_OType_OD = 0x01
} GPIOOType_TypeDef;

typedef enum 
{
    GPIO_Speed_Level_1 = 0x01,
    GPIO_Speed_Level_2 = 0x02,
    GPIO_Speed_Level_3 = 0x03
} GPIOSpeed_TypeDef;

typedef enum 
{
    GPIO_PuPd_NOPULL = 0x00,
    GPIO_PuPd_UP     = 0x01,
    GPIO_PuPd_DOWN   = 0x02
} GPIOPuPd_TypeDef;

typedef enum 
{
    GPIO_AF_0 = 0x00,
    GPIO_AF_1 = 0x01,
    GPIO_AF_2 = 0x02
} GPIOAF_TypeDef;

typedef enum 
{
    GPIO_PIN_2 = 0x02,
    GPIO_PIN_3 = 0x03,
    GPIO_PIN_15 = 0x0F
} GPIOPinPos_TypeDef;
/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/
void GpioInit(void);
void GpioSet(void);
void GpioReset(void);
#endif

/* end of Gpio.h */