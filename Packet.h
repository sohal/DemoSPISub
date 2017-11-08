/******************************************************************************/
/**
* @file Packet.h
* @brief Definition of the 68-byte data packet and its' status
*
*******************************************************************************/
#ifndef PACKET_H
#define PACKET_H
/* ***************** Header / include files ( #include ) **********************/
/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/**
* @struct tDATA_PACKET
* @brief Packet includes 64-byte blocks of data plus two-byte sequence count and two-byte CRC
*/
typedef struct
{
    uint8_t  u8Data[64];  /**< Data block     */
    uint16_t u16SeqCnt;  /**< Sequence count */
    uint16_t u16CRC;     /**< Two-byte CRC   */
}tDATA_PACKET;

/**
* @enum ePACKET_STATUS
* @brief Status of the data packet.
*/
typedef enum
{
    ePACKET_Ok           = 0,  /**< No error              */
    ePACKET_CRCError     = 1,  /**< CRC mismatch          */
    ePACKET_SNError      = 2   /**< Sequence number error */
}ePACKET_STATUS;

/* ***************** Global data declarations ( extern ) **********************/
/* ***** External parameter / constant declarations ( extern const ) **********/
/* ********************** Global func/proc prototypes *************************/
void PacketInit(void);
ePACKET_STATUS PacketProcess(tDATA_PACKET packet);
#endif

/* end of Packet.h */