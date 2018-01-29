/******************************************************************************/
/**
* @file Timeout.h
* @brief Definition of timeout values
*
*******************************************************************************/
#ifndef TIMEOUT_H
#define TIMEOUT_H

/* ***************** Header / include files ( #include ) **********************/
/* *************** Constant / macro definitions ( #define ) *******************/
#define TIMEOUT_1s                380810
#define TIMEOUT_263ms             100000
#define TIMEOUT_100ms             38081
#define TIMEOUT_10ms              3808
#define PACKET_TIMEOUT_UART_MS    TIMEOUT_100ms  /**< Maximum duration in ms of a packet + safety margin   */
#define PACKET_TIMEOUT_CAN_MS     TIMEOUT_100ms
#define PACKET_TIMEOUT_SPI_MS     TIMEOUT_100ms
#define WAIT_CMD_TIMEOUT_S        5  /**< 1s after power up start application if no bootloader cmd comes   */
#define BOOTLOADER_TIMEOUT_S      5  /**< The time the bootloader stays in “Wait for Packet” state         */
#define SENDER_TIMEOUT_MS         0  /**< The sender will resend the last packet if no status was received */

/* ********************* Type definitions ( typedef ) *************************/
/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/

#endif

/* end of Timeout.h */
