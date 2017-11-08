/******************************************************************************/
/**
* @file Command.h
* @brief Definition of commands
*
*******************************************************************************/
#ifndef COMMAND_H
#define COMMAND_H
/* ***************** Header / include files ( #include ) **********************/
/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/**
* @enum eCOMMAND_ID
* @brief Send commands.
*/
typedef enum
{
    eCMD_EraseFlash     = 0x01FE, /**< Erase current firmware in flash                                       */
    eCMD_WriteMemory    = 0x02FD, /**< Switch to bootloader mode to expect data packets for writing to flash */
    eCMD_BootloadMode   = 0x03FC, /**< Needs to come within 1s after start up to stay in bootloader mode     */
    eCMD_Finish         = 0x04FB, /**< End of bootloader mode; jump to application code                      */
    eCMD_NotValid       = 0x0000  /**< */
}eCOMMAND_ID;

/**
* @enum eRESPONSE_ID
* @brief Reply response.
*/
typedef enum
{
    eRES_Ready     = 0xF10E, /**< Successfully switched to bootloader mode         */
    eRES_Abort     = 0xF20D, /**< Not recoverable error happened, only can abort   */    
    eRES_OK        = 0xF30C, /**< Last command was processed correctly             */
    eRES_Error     = 0xF40B  /**< Last command had an error                        */
}eRESPONSE_ID;

/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/

#endif

/* end of Command.h */
