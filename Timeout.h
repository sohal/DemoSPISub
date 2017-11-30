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
#define PACKET_TIMEOUT_UART_MS    0  /**< Maximum duration in ms of a packet + safety margin               */
#define PACKET_TIMEOUT_CAN_MS     0
#define PACKET_TIMEOUT_SPI_MS     0
#define WAIT_CMD_TIMEOUT_S        5  /**< 1s after power up start application if no bootloader cmd comes   */
#define BOOTLOADER_TIMEOUT_S      5  /**< The time the bootloader stays in “Wait for Packet” state         */
#define SENDER_TIMEOUT_MS         0  /**< The sender will resend the last packet if no status was received */

/* ********************* Type definitions ( typedef ) *************************/
/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/

#endif

/* end of Timeout.h */
