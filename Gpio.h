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
    GPIO_Mode_IN  = 0,
    GPIO_Mode_OUT,
    GPIO_Mode_AF,
    GPIO_Mode_AN
} GPIOMode_TypeDef;

typedef enum 
{
    GPIO_OType_PP = 0,
    GPIO_OType_OD
} GPIOOType_TypeDef;

typedef enum 
{
    GPIO_Speed_Level_1 = 1,
    GPIO_Speed_Level_2,
    GPIO_Speed_Level_3
} GPIOSpeed_TypeDef;

typedef enum 
{
    GPIO_PuPd_NOPULL = 0,
    GPIO_PuPd_UP,
    GPIO_PuPd_DOWN
} GPIOPuPd_TypeDef;

typedef enum 
{
    GPIO_AF_0 = 0,
    GPIO_AF_1,
    GPIO_AF_2,
    GPIO_AF_3,
    GPIO_AF_4
} GPIOAF_TypeDef;

typedef enum {
	GPIO_PIN_0  = 0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
	GPIO_PIN_8,
	GPIO_PIN_9,
	GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
	GPIO_PIN_13,
	GPIO_PIN_14,
    GPIO_PIN_15
} GPIOPinPos_TypeDef;

#endif

/* end of Gpio.h */
