#ifndef PROTO_SM
#define PROTO_SM

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "Common.h"
#include "Usart1.h"
#include "Command.h"
#include "Flash.h"
#include "Packet.h"
#include "Timer.h"
#include "BSP.h"


typedef union myCmd{
	uint8_t 				bufferCMD[2];
	eRESPONSE_ID 		returnValue;
	eCOMMAND_ID 		receivedvalue;
}tCmdUnion;

typedef union myPayload{
	tDATA_PACKET 		packet;
	uint8_t 				bufferPLD[68];
}tPldUnion;

typedef union myAppData{
	tFIRMWARE_PARAM Firmware;
	uint8_t 				bufferData[4];
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
	eFlashVerifyApplication,
	eStartAppCMD
}tProtoState;

eFUNCTION_RETURN ProtocolSM_Run(const tBSPStruct *);

#endif
