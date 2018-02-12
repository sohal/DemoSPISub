
/******************************************************************************/
/**
* @file ProtocolStateMachine.c
* @brief Implement State machine for protocol handling of bootloader
*
*******************************************************************************/

/* ***************** Header / include files ( #include ) **********************/
#include "ProtoStateMachine.h"
/* *************** Constant / macro definitions ( #define ) *******************/
/* ********************* Type definitions ( typedef ) *************************/
/* *********************** Global data definitions ****************************/
/* **************** Global constant definitions ( const ) *********************/
/* ***************** Modul global data segment ( static ) *********************/
static tCmdUnion 					Command;
static tPldUnion 					Payload;
static tAppDataUnion 			AppData;
static volatile uint32_t *AppVectorsInFlash 	= (volatile uint32_t *)BSP_APPLICATION_START;
static volatile uint32_t *AppVectorsInRAM 		= (volatile uint32_t *)BSP_SRAM_START;
/* *************** Modul global constants ( static const ) ********************/
/* **************** Local func/proc prototypes ( static ) *********************/


/******************************************************************************/
/**
* eFUNCTION_RETURN ProtocolSM_Run(const tBSPStruct *pBSP)
* @brief 	Main function to to bootload the new application from interface for
*					the selected BSP.
*
* @param[in] contant pointer to the BSP structure decided as per Hardware
* @returns   eFunction_OK if all ok
*            or
*            error otherwise.
*
*******************************************************************************/
eFUNCTION_RETURN ProtocolSM_Run(const tBSPStruct *pBSP)
{
	eFUNCTION_RETURN retVal = eFunction_Ok;
	static tProtoState stateNow, stateNext;
	static uint16_t pktCounter;
	static uint16_t crcCalculated = 0xFFFF;
	static uint16_t crcPrevious = 0x0000;
	static uint32_t tickCounter = 0U;
	static uint32_t stickyTimer = 0U;
		
	switch(stateNow)
	{
		case eDefaultState:
			if(pBSP->pRecv(Command.bufferCMD, 2) == eFunction_Ok)
			{
				if(Command.receivedvalue == eCMD_BootloadMode)
				{
					stateNext = eFlashEraseCMD;
					tickCounter = 0;
					Command.returnValue = eRES_Ready;
					pBSP->pSend(Command.bufferCMD,2);
				}
			}else
			{
				if(tickCounter > pBSP->AppStartTicks)
				{
					stateNext = eFlashVerifyApplication;
					tickCounter = 0;
				}else
				{
					tickCounter++;
					stateNext = eDefaultState;
				}
			}
			break;
			
		case eFlashEraseCMD:
			if(pBSP->pRecv(Command.bufferCMD, 2) == eFunction_Ok)
			{
				if(Command.receivedvalue == eCMD_EraseFlash)
				{
					FlashErase();
					stateNext = eWriteMemory;
					Command.returnValue = eRES_OK;
					pBSP->pSend(Command.bufferCMD, 2);
				}
			}else
			{
				stateNext = eFlashEraseCMD;
			}
			break;
		
		case eWriteMemory:
			if(pBSP->pRecv(Command.bufferCMD, 2) == eFunction_Ok)
			{
				if(Command.receivedvalue == eCMD_WriteMemory)
				{
					stateNext = ePayloadReceive;
					Payload.packet.u16SeqCnt = 0xFFFF;
					pktCounter = 0;
					Command.returnValue = eRES_OK;
					pBSP->pSend(Command.bufferCMD, 2);
				}
			}else
			{
				stateNext = eWriteMemory;
			}
			break;
			
		case ePayloadReceive:
			retVal = pBSP->pRecv(Payload.bufferPLD, sizeof(tPldUnion));
			if((pktCounter == Payload.packet.u16SeqCnt) && (retVal == eFunction_Ok))
			{
				stateNext = ePayloadCheck;
				tickCounter = 0;
			}
			if(Payload.packet.u16CRC == 0xFFFF)
			{
				if(Payload.packet.u16SeqCnt == 0xFFFF)
				{
					if(Payload.packet.u8Data[0] == 0x04)
					{
						if(Payload.packet.u8Data[1] == 0xFB)
						{
							if(tickCounter > pBSP->TwoBytesTicks)
							{
								stateNext = eCrcCMD;
								AppData.Firmware.u16FWCRC = 0xFFFF;
								AppData.Firmware.u16FWLen = 0xFFFF;
							}else
							{
								tickCounter++;
							}
						}
					}
				}
			}
			break;
		
		case ePayloadCheck:
			crcCalculated = CRCCalc16(Payload.packet.u8Data,64, crcPrevious); 
				if(crcCalculated == Payload.packet.u16CRC)
				{
					if(FlashWrite(Payload.bufferPLD, BLOCK_SIZE))
          {
						if(FlashVerify(Payload.bufferPLD, BLOCK_SIZE))
            {
							Command.returnValue = eRES_OK;
							pktCounter++;
							crcPrevious = crcCalculated;
            }else
           {
							Command.returnValue = eRES_Abort;
            }
					}
				stateNext = ePayloadReceive;
				Payload.packet.u16SeqCnt = 0xFFFF;
				Payload.packet.u16CRC = 0xFFFF;
				pBSP->pSend(Command.bufferCMD, 2);
			}
			break;

		case eCrcCMD:
			stateNext = eWriteApplicationLen;
			pBSP->pReset();
			Command.returnValue = eRES_OK;
			pBSP->pSend(Command.bufferCMD,2);
			break;

		case eWriteApplicationLen:
			retVal = pBSP->pRecv(AppData.bufferData, 4);
			if(retVal == eFunction_Ok)
			{
				
				Command.returnValue = eRES_Abort;
				if(FlashWriteFWParam(AppData.Firmware))
				{
					if(FlashVerifyFWParam(AppData.Firmware))
					{
						stateNext = eFinishUpdate;
						pBSP->pReset();
						Command.returnValue = eRES_OK;
					}
				}
				pBSP->pSend(Command.bufferCMD,2);
			}
			break;
		
		case eFinishUpdate:
			retVal = pBSP->pRecv(Command.bufferCMD, 2);
			if((retVal == eFunction_Ok) && (Command.receivedvalue == eCMD_Finish))
			{				
				stateNext = eFlashVerifyApplication;
			}
			break;
		
		case eFlashVerifyApplication:
			Command.returnValue = eRES_Abort;
			if(FlashVerifyFirmware())
			{
				Command.returnValue = eRES_OK;
				stateNext = eStartAppCMD;
			}else
			{
				Command.returnValue = eRES_AppCrcErr;
				stateNext = eDefaultState;
			}
			pBSP->pSend(Command.bufferCMD, 2);
			break;
		
		case eStartAppCMD:
			stickyTimer = pBSP->CommDoneTicks;
			do{
			}while(stickyTimer--);
		
			pBSP->pDeInit();
				
			/* Lock flash from further write */				
      FlashLock();
			/* Remap Application Vectors */
			for(int i = 0; i < BSP_VECTOR_SIZE_WORDS; i++)
			{
        AppVectorsInRAM[i] = AppVectorsInFlash[i];
			}
			/* Setup controller mode to consider vectors from RAM */
			RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
			SYSCFG->CFGR1 |= SYSCFG_CFGR1_MEM_MODE;  																	
			
			/* Setup Application Stack Pointer */
			__set_MSP(*AppVectorsInFlash);
			/* Setup Program counter with the start of Application */
			((void (*)(void))*(AppVectorsInFlash + 1UL))();  
			break;
			
		default:
			/* We will never come here if all is OK, nevertheless we restore state machine to default state */
			stateNext = eDefaultState;
			break;
	}
	
	/* Check if the same state is repeating, no transition suggests a
	 * hung state. We can count the stickyness of the software and reset
	 * to a known state.
	 */
	if(stateNext == stateNow)
	{
		stickyTimer++;
		/* If the timeout has expired, we reboot the system */
		if(stickyTimer > pBSP->ThreeSecondsTicks)
		{
			stateNext = eDefaultState;
			NVIC_SystemReset();
			stickyTimer = 0U;
		}
	}else
	{
		/* Reset the sticky counter if the state transition takes place */
		stickyTimer = 0U;
	}
	
	stateNow = stateNext;
	
	return(retVal);
}
