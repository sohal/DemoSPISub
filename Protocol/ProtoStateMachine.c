
#include "ProtoStateMachine.h"

tCmdUnion Command;
tPldUnion Payload;
tAppDataUnion AppData;
extern uint32_t __Vectors_Size;
volatile uint32_t *__vectors = (volatile uint32_t *)FLASH_PROGRAM_START_ADDRESS;
volatile uint32_t *__ram = (volatile uint32_t *)SRAM_START_ADDRESS;
			
			
eFUNCTION_RETURN ProtocolSM_Run(void)
{
	eFUNCTION_RETURN retVal = eFunction_Ok;
	static tProtoState stateNow, stateNext;
	static uint16_t pktCounter;
	static uint16_t crcCalculated = 0xFFFF;
	static uint16_t crcPrevious = 0x0000;
	static uint32_t tickCounter = 0U;
	
	switch(stateNow)
	{
		case eDefaultState:
			if(Usart1Receive(Command.bufferCMD, 2) == eFunction_Ok)
			{
				if(Command.receivedvalue == eCMD_BootloadMode)
				{
					stateNext = eFlashEraseCMD;
					tickCounter = 0;
					Command.returnValue = eRES_Ready;
					Usart1Transmit(Command.bufferCMD,2);
				}
			}else
			{
				if(tickCounter > 300000)
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
			if(Usart1Receive(Command.bufferCMD, 2) == eFunction_Ok)
			{
				if(Command.receivedvalue == eCMD_EraseFlash)
				{
					FlashErase();
					stateNext = eWriteMemory;
					Command.returnValue = eRES_OK;
					Usart1Transmit(Command.bufferCMD, 2);
				}
			}else
			{
				stateNext = eFlashEraseCMD;
			}
			break;
		
		case eWriteMemory:
			if(Usart1Receive(Command.bufferCMD, 2) == eFunction_Ok)
			{
				if(Command.receivedvalue == eCMD_WriteMemory)
				{
					stateNext = ePayloadReceive;
					Payload.packet.u16SeqCnt = 0xFFFF;
					pktCounter = 0;
					Command.returnValue = eRES_OK;
					Usart1Transmit(Command.bufferCMD, 2);
				}
			}else
			{
				stateNext = eWriteMemory;
			}
			break;
			
		case ePayloadReceive:
			retVal = Usart1Receive(Payload.bufferPLD, sizeof(tPldUnion));
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
							if(tickCounter > 3000)
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
				Usart1Transmit(Command.bufferCMD, 2);
			}
			break;

		case eCrcCMD:
			stateNext = eWriteApplicationLen;
			Usart1ReceiveReady();
			Command.returnValue = eRES_OK;
			Usart1Transmit(Command.bufferCMD,2);
			break;

		case eWriteApplicationLen:
			retVal = Usart1Receive(AppData.bufferData, 4);
			if(retVal == eFunction_Ok)
			{
				
				Command.returnValue = eRES_Abort;
				if(FlashWriteFWParam(AppData.Firmware))
				{
					if(FlashVerifyFWParam(AppData.Firmware))
					{
						stateNext = eFinishUpdate;
						Usart1ReceiveReady();
						Command.returnValue = eRES_OK;
					}
				}
				Usart1Transmit(Command.bufferCMD,2);
			}
			break;
		
		case eFinishUpdate:
			retVal = Usart1Receive(Command.bufferCMD, 2);
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
			Usart1Transmit(Command.bufferCMD,2);
			break;
		
		case eStartAppCMD:
			LED_Off();
      FlashLock();
			for(int i = 0; i < 0x30 /*((uint32_t)&__Vectors_Size / sizeof(uint32_t))*/; i++)
			{
        __ram[i] = __vectors[i];
			}

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_MEM_MODE;  // remap exception vectors

    __set_MSP(*__vectors);  // setup stack pointer
    ((void (*)(void))*(__vectors + 1))();  // jump to reset handler
			break;
			
		default:
			//stateNext = 0;
			break;
	}
	
	stateNow = stateNext;
	
	return(retVal);
}
