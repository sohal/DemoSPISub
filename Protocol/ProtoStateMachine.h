#ifndef PROTO_SM
#define PROTO_SM

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <Common.h>
#include <Usart1.h>
#include "LED.h"
#include "Command.h"
#include "Flash.h"
#include "Packet.h"
#include "Timer.h"
#ifndef SRAM_START_ADDRESS
#define 	SRAM_START_ADDRESS  0x20000000
#endif
typedef union myCmd{
	uint8_t bufferCMD[2];
	eRESPONSE_ID returnValue;
	eCOMMAND_ID receivedvalue;
}tCmdUnion;

typedef union myPayload{
	tDATA_PACKET packet;
	uint8_t bufferPLD[68];
}tPldUnion;

typedef union myAppData{
	tFIRMWARE_PARAM Firmware;
	uint8_t bufferData[4];
}tAppDataUnion;

typedef enum {
	eDefaultState = 0,
	eBootCMD,
	eFlashEraseCMD,
	eWriteMemory,
	ePayloadReceive,
	ePayloadCheck,
	eWriteApplication,
	eCrcCMD,
	eWriteApplicationLen,
	eFinishUpdate,
	eCrcFWConfirm,
	eStartAppCMD
}tProtoState;

eFUNCTION_RETURN ProtocolSM_Run(void);

#endif
