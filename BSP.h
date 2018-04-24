/******************************************************************************/
/**
* @file BSP.h
* @brief Implement BSP
*
*******************************************************************************/
#ifndef BSP_H_
#define BSP_H_
/* ***************** Header / include files ( #include ) **********************/
#include "stm32f0xx.h"
#include "Common.h"
/* *************** Constant / macro definitions ( #define ) *******************/

#define BootTIMEOUT                         (300000U)
#define BSP_ALLBOARD_HSI_FREQUENCY          (8000000U)

/** Constants for Chip ID */
#define DBGMCU_ID_F04x                      (0x00000445UL)
#define DBGMCU_ID_F03x                      (0x00000444UL)

/** Constants related to Program flash in the µC */
#define BSP_ABSOLUTE_FLASH_START            (0x08000000UL)  /** Start address of Program Flash */
#define BSP_FLASH_PAGE_SIZE_BYTES           (0x400UL)       /** Page size of Program Flash in bytes */

/** Constants related to SRAM in the µC */
#define BSP_ABSOLUTE_SRAM_START             (0x20000000UL)  /** Start address of SRAM */

/** Constants related to relocation of interrupt vectors of application in SRAM */
#define BSP_APP_VECTOR_SIZE_BYTES           (0x000000C0UL)
#define BSP_APP_VECTOR_SIZE_WORDS           (BSP_APP_VECTOR_SIZE_BYTES / sizeof(uint32_t))

/** Constants related to Bootloader in program flash */
/** Maximum allowed size of Bootloader */
#define BSP_BOOTLOADER_MAX_SIZE             (0x1000UL)

/** Constants related to Application in program flash */
/** The start of any application is always fixed in flash at 0x08001000UL */
#define BSP_ABSOLUTE_APP_START              (BSP_ABSOLUTE_FLASH_START + BSP_BOOTLOADER_MAX_SIZE)

/** Constants related to Pilot(UID) STM32F031G4U6 with 16kB flash */
/** Total capacity of Program Flash in STM32F031G4U6 */
#define BSP_FLASH_MAX_SIZE_16KB             (0x4000UL)
/** End of Flash address in STM32F031G4U6 */
#define BSP_ABSOLUTE_FLASH_END_16KB         (0x08004000UL)
/** Total flash sectors to be erased in 16kB flash for application space (0x0C)*/
#define BSP_APP_PROGRAM_PAGES_16KB          ((BSP_FLASH_MAX_SIZE_16KB - BSP_BOOTLOADER_MAX_SIZE) / BSP_FLASH_PAGE_SIZE_BYTES)

/** Constants related to other targets using STM32F031G6U6 and STM32F042K6U6 with 32kB flash */
/** Total capacity of Program Flash in STM32F031G6U6 and STM32F042K6U6 */
#define BSP_FLASH_MAX_SIZE_32KB             (0x8000UL)
/** End of Flash address in STM32F031G6U6 and STM32F042K6U6 */
#define BSP_ABSOLUTE_FLASH_END_32KB         (0x08008000UL)
/** Total flash sectors to be erased in 32kb flash for application space (0x1C) */
#define BSP_APP_PROGRAM_PAGES_32KB          ((BSP_FLASH_MAX_SIZE_32KB - BSP_BOOTLOADER_MAX_SIZE) / BSP_FLASH_PAGE_SIZE_BYTES)

/** Constants for interfaces */
/** Interface Ports, Pins and configuration in Pilot for UART communication */
#define BSP_PILOT_UART_PORT                 (GPIOA)
#define BSP_PILOT_UART_TX_PIN               (2U)
#define BSP_PILOT_UART_RX_PIN               (3U)
#define BSP_PILOT_UART_BAUD                 (57600U)

/** Interface Ports, Pins and configuration in Torque sensor for UART communication */
#define BSP_TORQUE_UART_PORT                (GPIOA)
#define BSP_TORQUE_UART_TX_PIN              (2U)
#define BSP_TORQUE_UART_RX_PIN              (3U)
#define BSP_TORQUE_UART_BAUD                (1125000U)
#define BSP_TORQUE_RCC_MULTIPLIER           (8UL)

/** Interface Ports, Pins and configuration in targets for CAN bus communication */
#define BSP_TARGET_CAN_PORT                 (GPIOA)
#define BSP_TARGET_CAN_TX_PIN               (12U)
#define BSP_TARGET_CAN_RX_PIN               (11U)
#define BSP_TARGET_CAN_BAUD                 (500000U)
#define BSP_TARGET_RCC_MULTIPLIER           (8UL)
#define BSP_TARGET_CAN_ID_BASE              (8U)

#define BSP_NUCLEO_UART_PORT                (GPIOA)
#define BSP_NUCLEO_UART_TX_PIN              (2U)
#define BSP_NUCLEO_UART_RX_PIN              (15U)
#define BSP_NUCLEO_UART_BAUD                (57600U)

#define BSP_CHECK_PORT                      (GPIOA)
#define BSP_CHECK_PIN_6                     (6U)
#define BSP_CHECK_PIN_7                     (7U)
/* ********************* Type definitions ( typedef ) *************************/
typedef struct timeouts{
    uint32_t ThreeSeconds;
    uint32_t StartApp;
    uint32_t EndOfComm;
}tTimeouts;

typedef enum bsptype {
    BSP_UnKnown,
    BSP_Pilot,
    BSP_TorqueSensor,
    BSP_ExtWatchdog,
    BSP_CAN,
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
/*******************************************************************************/
tBSPStruct* BSP_Init(void);
void BSP_DeInit(void);
#endif
