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

/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/

#endif

/* end of Packet.h */
