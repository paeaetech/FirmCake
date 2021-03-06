
#include <util/crc16.h>
#include "config.h"
#include "State.h"
#include "HostComm.h"
#include "uart.h"
#include "Packet.h"
#include "StepperController.h"
#include "rs485.h"
#include "utils.h"
#include "version.h"
#include "owntypes.h"
#include "fatfs/pff.h"
#include "eepromConfig.h"
#include "eeprom.h"
#include "SDCard.h"

#if PROTOCOL_INTERPRETER == PROTOCOL_GCODE
#include "GCodeParser.h"
#endif

HostComm hostComm;

HostComm::HostComm()
: mPacket(mReceiveBuffer,HOST_RECEIVE_BUFFER_SIZE), mReplyPacket(mReplyBuffer,HOST_REPLY_BUFFER_SIZE), 
  mSlavePacket(mSlaveBuffer,HOST_SLAVE_BUFFER_SIZE), mSlaveReplyPacket(mSlaveReplyBuffer,HOST_SLAVE_REPLY_BUFFER_SIZE),
  mCommandBuffer(mCommandBuf,COMMAND_BUFFER_SIZE), mCommandsProcessed(0), mState(WAIT_PACKET)
{
	
}

void HostComm::update()
{
	if (mState != WAIT_PACKET)
	{
		if (millis() - lastMillis > HOST_TIMEOUT)
		{
			//handle timeout
			sendReply(HOST_REPLY_TIMEOUT);
			mState = WAIT_PACKET;
		}
	}
	
	while(uart0.available())
	{
#if PROTOCOL_INTERPRETER == PROTOCOL_SANGUINO3G
		LED_ON();
		uint8_t b = uart0.receive();
		lastMillis = millis();
		switch(mState)
		{
			case WAIT_PACKET:
				if (b == PACKET_START_BYTE)
				{
					mState=PACKET_LEN;
					mPacket.reset();
				}
				break;
			case PACKET_LEN:
				mPacketLen = b;
				mCrc = 0;
				mState = PACKET_PAYLOAD;
				break;
			case PACKET_PAYLOAD:
				mPacket.processData(b);
				mCrc = _crc_ibutton_update(mCrc,b);
				mPacketLen--;
				if (!mPacketLen)
				{
					mState = PACKET_CRC;
				}
				break;
			case PACKET_CRC:
				if (mCrc != b)
					sendReply(HOST_REPLY_INVALID_CRC);
				else
				{
					processPacket();
				}
				mState = WAIT_PACKET;
				break;
		}
		LED_OFF();
#elif PROTOCOL_INTERPRETER == PROTOCOL_GCODE
		LED_ON();
		uint8_t b = uart0.receive();
		if (gcodeParser.processByte(b))
		{
			mPacket.reset();
			if (gcodeParser.getPacket(mPacket))
				processPacket();
		}
		LED_OFF();
#else
#error invalid protocol selected
#endif
		
	}
}

void HostComm::copyPacketTo(Packet& rPacket,Packet &rDestPacket)
{
	while(rPacket.getBytesRemaining())
	{
		rDestPacket.put8(rPacket.get8());
	}
}

void HostComm::processPacket()
{
	mReplyPacket.reset();
	mReplyPacket.setCommand(HOST_REPLY_OK);
	
	mPacket.setCommand(mPacket.get8());
	bool isCommand = mPacket.getCommand() & 0x80;
	
	if (isCommand)
	{
		if (mCommandBuffer.getSpaceAvailable() >= mPacket.getBytesRemaining()+1) //+1 for command
		{
			mCommandBuffer.put(mPacket.getCommand());

			while(mPacket.getBytesRemaining())
				mCommandBuffer.put(mPacket.get8());
		}
		else
		{
			sendReply(HOST_REPLY_BUFFER_OVERFLOW);
			return;
		}
	}
	else
	{
		switch((HostCommand)mPacket.getCommand())
		{
			case HOST_CMD_VERSION:
			{
				uint16_t v = mPacket.get16();
				DEBUG_OUTF("CMD_VERSION: %d.%d\r\n",v/100,v%100);
				mReplyPacket.put16(303); //fake version to replicatorg
				break;
			}
			case HOST_CMD_INIT:
				break;
			case HOST_CMD_GET_BUFFER_SIZE:
				mReplyPacket.put16(mCommandBuffer.getSpaceAvailable());
				DEBUG_OUTF("CMD_GET_BUFFER_SIZE: %d\r\n",mCommandBuffer.getSpaceAvailable());
				break;
			case HOST_CMD_CLEAR_BUFFER:
				DEBUG_OUT("CMD_CLEAR_BUFFER\r\n");
				mCommandBuffer.reset();
				break;
			case HOST_CMD_GET_POSITION:
			{
				Point p;
				stepperController.getPoint(p);
				mReplyPacket.put32(p.x);
				mReplyPacket.put32(p.y);
				mReplyPacket.put32(p.z);
				break;
			}
			case HOST_CMD_GET_RANGE:
			{
				Point p;
				stepperController.getRange(p);
				mReplyPacket.put32(p.x);
				mReplyPacket.put32(p.y);
				mReplyPacket.put32(p.z);
				break;
			}
			case HOST_CMD_SET_RANGE:
			{
				Point p;
				p.x = mPacket.get32();
				p.y = mPacket.get32();
				p.z = mPacket.get32();
				stepperController.setRange(p);
				break;
			}
			case HOST_CMD_RESET:
				DEBUG_OUT("CMD_RESET\r\n");
				sendReply();
				reset();
				break;

			case HOST_CMD_TOOL_QUERY:
			{
				DEBUG_OUT("CMD_TOOL_QUERY\r\n");
				mSlavePacket.reset();
				while(mPacket.getBytesRemaining())
				{
					mSlavePacket.put8(mPacket.get8());
				}
				if (!sendSlaveQuery())
				{
					mReplyPacket.reset();
					mReplyPacket.setCommand(HOST_REPLY_TIMEOUT);
				}
				break;
			}
			case HOST_CMD_IS_FINISHED:
			{
				bool finished = stepperController.isMoving() != true && mCommandBuffer.available() == 0;
				mReplyPacket.put8(finished ? 1 : 0);
				break;
			}
			case HOST_CMD_READ_EEPROM:
			{
				uint16_t offset = mPacket.get16();
				uint8_t count = mPacket.get8();
				DEBUG_OUTF("CMD_READ_EEPROM offset=%d, count=%d\r\n",offset,count);
				if (count > 16)
					mReplyPacket.setCommand(HOST_REPLY_BUFFER_OVERFLOW);
				else
				{
					if (!eepromConfigValid())
					{
						for (uint8_t i=0;i<count;i++)
							mReplyPacket.put8(0);
					}
					else
					{
						for (uint8_t i=0;i<count;i++)
							mReplyPacket.put8(eeprom_read(offset+i));
					}
				}
				break;
			}
			case HOST_CMD_WRITE_EEPROM:
			{
				uint16_t offset = mPacket.get16();
				uint8_t count = mPacket.get8();
				DEBUG_OUTF("CMD_WRITE_EEPROM offset=%d, count=%d\r\n",offset,count);
				for (uint8_t i=0;i<count;i++)
				{
					eeprom_write(offset+i,mPacket.get8());
				}
				mReplyPacket.put8(count);
				break;
			}
			case HOST_CMD_ABORT:
			case HOST_CMD_PAUSE:
				break;
#ifdef USE_SDCARD
		  	case HOST_CMD_CAPTURE_TO_FILE:
			case HOST_CMD_END_CAPTURE:
		  	case HOST_CMD_PLAYBACK_CAPTURE:
			case HOST_CMD_NEXT_FILENAME:
			{
				uint8_t reset = mPacket.get8();
				if (reset)
				{
					SDCardResponse ret = sdCard.resetScan();
					mReplyPacket.put8(ret);
					mReplyPacket.put8(0);
				}
				else
				{
					uint8_t buf[16];
					SDCardResponse ret = sdCard.getNextFile(buf,sizeof(buf));
					uint8_t* p = &buf[0];
					mReplyPacket.put8(ret);
					while(*p)
						mReplyPacket.put8(*p++);
					mReplyPacket.put8(0);
				}
				break;
			}
#endif
			case HOST_CMD_GET_DBG_REG:

			case HOST_CMD_PROBE:
			default:
				mReplyPacket.setCommand(HOST_REPLY_UNSUPPORTED);
				break;
		}
	}
	sendReply();

}

void HostComm::processCommandBuffer()
{
	if (mCommandBuffer.available())
	{
	 	HostCommand cmd = (HostCommand)mCommandBuffer.get8();
		switch(cmd)
		{
			case HOST_CMD_QUEUE_POINT_ABS:
			{
				if (!stepperController.isMoving())
				{
					int32_t x = (int32_t)mCommandBuffer.get32();
			        int32_t y = (int32_t)mCommandBuffer.get32();
			        int32_t z = (int32_t)mCommandBuffer.get32();
			        int32_t feedRate = (int32_t)mCommandBuffer.get32();
						
					DEBUG_OUTF("HOST_CMD_QUEUE_POINT_ABS %x %x %x %x\r\n",x,y,z,feedRate);

					stepperController.moveTo(x,y,z,feedRate);
				}
				else
					mCommandBuffer.undo8();
				break;
	      	}
			case HOST_CMD_SET_POSITION:
			{
				if (!stepperController.isMoving())
				{
					int32_t x = (int32_t)mCommandBuffer.get32();
			        int32_t y = (int32_t)mCommandBuffer.get32();
			        int32_t z = (int32_t)mCommandBuffer.get32();
					DEBUG_OUTF("HOST_CMD_SET_POSITION %d %d %d\r\n",x,y,z);

					stepperController.setPoint(x,y,z);
				}
				else
					mCommandBuffer.undo8();
				break;
			}
			case HOST_CMD_FIND_AXES_MINIMUM:
			case HOST_CMD_FIND_AXES_MAXIMUM:
			{
				uint8_t flags = mCommandBuffer.get8();
				uint32_t feedRate = mCommandBuffer.get32();
				uint16_t timeout = mCommandBuffer.get16();
				break;
			}
			case HOST_CMD_DELAY:
			{
				uint32_t delayMs = mCommandBuffer.get32();
				DEBUG_OUTF("HOST_CMD_DELAY %f\r\n",delayMs);
				initDelayState(delayMs);
				break;
			}
			case HOST_CMD_CHANGE_TOOL:
			{
				uint8_t tool = mCommandBuffer.get8();
				break;
			}
			case HOST_CMD_WAIT_FOR_TOOL:
			{
				if (!stepperController.isMoving())
				{
					uint8_t tool = mCommandBuffer.get8();
					uint16_t pingDelay = mCommandBuffer.get16();
					uint32_t timeout = mCommandBuffer.get16();
					mSlaveWaitTimeout = timeout*1000+millis();
					mainState = STATE_WAIT_FOR_SLAVE;
				}
				else
					mCommandBuffer.undo8();
				break;
			}
			case HOST_CMD_TOOL_COMMAND:
			{
				mReplyPacket.reset();
				mReplyPacket.setCommand(HOST_REPLY_OK);
				
				DEBUG_OUT("HOST_CMD_TOOL_COMMAND\r\n");
				mSlavePacket.reset();
				mSlavePacket.put8(mCommandBuffer.get()); //tool id
				mSlavePacket.put8(mCommandBuffer.get()); //command
				uint8_t len = mCommandBuffer.get();
				for (uint8_t i=0;i<len;i++)
				{
				        mSlavePacket.put8(mCommandBuffer.get());
				}
				sendSlaveQuery();
				break;
			}
			case HOST_CMD_ENABLE_AXES:
			{
				if (!stepperController.isMoving())
				{
					uint8_t axes = mCommandBuffer.get8();
					bool x = axes & _BV(1);
					bool y = axes & _BV(2);
					bool z = axes & _BV(3);
					bool enable = axes & 0x80;
					DEBUG_OUTF("HOST_CMD_ENABLE_AXES %s %s%s%s\r\n",enable ? "enable" : "disable",x ? "x":"",y ? "y" : "",z ? "z" : "");
					if (enable)
						stepperController.enableSteppers(x,y,z);
					else
						stepperController.disableSteppers(x,y,z);
				}
				else
					mCommandBuffer.undo8();
				break;
			}
			default:
				DEBUG_OUTF("INVALID cmdbuffer command %d\r\n", cmd);
				break;
		}		
	}
}


void HostComm::sendReply()
{
	LED_ON();
	DEBUG_OUTF("Sending reply code %d\r\n",mReplyPacket.getCommand());

	uint8_t crc=_crc_ibutton_update(0,mReplyPacket.getCommand());
	uart0.send(PACKET_START_BYTE);
	uart0.send(mReplyPacket.getBytesRemaining()+1);
	uart0.send(mReplyPacket.getCommand());
	while(mReplyPacket.getBytesRemaining())
	{
		uint8_t b=mReplyPacket.get8();
		crc = _crc_ibutton_update(crc,b);
		uart0.send(b);
	}
	uart0.send(crc);
	LED_OFF();
}

void HostComm::sendReply(HostReply r)
{
	LED_ON();
	uart0.send(PACKET_START_BYTE);
	uart0.send(1); //length
	uart0.send((uint8_t)r);
	uart0.send(_crc_ibutton_update(0,r));
	LED_OFF();
}

void HostComm::sendSlavePacket()
{
	rs485.start();
	uint8_t crc=0;
	
	rs485.send(PACKET_START_BYTE);
	rs485.send(mSlavePacket.getBytesRemaining());
	
	while(mSlavePacket.getBytesRemaining())
	{
		uint8_t b = mSlavePacket.get8();
		rs485.send(b);
		crc = _crc_ibutton_update(crc,b);
	}
	rs485.send(crc);
	
	rs485.end();
	
}

bool HostComm::sendSlaveQuery()
{
#ifndef DEBUG_FAKE_SLAVE
	sendSlavePacket();
	
	if (readSlaveReply())
	{
		copyPacketTo(mSlaveReplyPacket,mReplyPacket);
		return true;
	}
		
	return false;
#else
	//pretend that we have a slave device connected, for debugging.
	uint8_t tool = mSlavePacket.get8();
	DEBUG_OUTF("Tool index %d\r\n",tool);
	SlaveCommand cmd = (SlaveCommand)mSlavePacket.get8();
	switch(cmd)
	{
		case SLAVE_CMD_VERSION:
			DEBUG_OUT("SLAVE_CMD_VERSION\r\n");
			mSlaveReplyPacket.put16(203);
			break;
		case SLAVE_CMD_INIT:
			DEBUG_OUT("SLAVE_CMD_INIT\r\n");
			break;
		case SLAVE_CMD_GET_TEMP:
			DEBUG_OUT("SLAVE_CMD_GET_TEMP\r\n");
			mSlaveReplyPacket.put16(23);
			break;
		case SLAVE_CMD_SET_TEMP:
		{
			uint16_t t=mSlavePacket.get16();
			DEBUG_OUTF("SLAVE_CMD_SET_TEMP t=%d\r\n",t);
			break;
		}
		case SLAVE_CMD_GET_PLATFORM_TEMP:
		{
			DEBUG_OUT("SLAVE_CMD_GET_PLATFORM_TEMP\r\n");
			mSlaveReplyPacket.put16(23);
			break;
		}
		case SLAVE_CMD_SET_PLATFORM_TEMP:
		{
			uint16_t t=mSlavePacket.get16();
			DEBUG_OUTF("SLAVE_CMD_SET_PLATFORM_TEMP t=%d\r\n",t);
			break;
		}
		case SLAVE_CMD_SET_MOTOR_1_PWM:
		{
			uint8_t t = mSlavePacket.get8();
			DEBUG_OUTF("SLAVE_CMD_SET_MOTOR_1_PWM t=%d\r\n",t);
			break;
		}	
		case SLAVE_CMD_SET_MOTOR_2_PWM:
		{
			uint8_t t = mSlavePacket.get8();
			DEBUG_OUTF("SLAVE_CMD_SET_MOTOR_2_PWM t=%d\r\n",t);
			break;
		}	
		case SLAVE_CMD_SET_MOTOR_1_RPM:
		{
			uint32_t t = mSlavePacket.get32();
			DEBUG_OUTF("SLAVE_CMD_SET_MOTOR_1_RPM t=%d\r\n",t);
			break;
		}	
		case SLAVE_CMD_SET_MOTOR_2_RPM:
		{
			uint32_t t = mSlavePacket.get32();
			DEBUG_OUTF("SLAVE_CMD_SET_MOTOR_2_RPM t=%d\r\n",t);
			break;
		}	
		case SLAVE_CMD_SET_MOTOR_1_DIR:
		{
			uint8_t t = mSlavePacket.get8();
			DEBUG_OUTF("SLAVE_CMD_SET_MOTOR_1_DIR t=%s\r\n",t ? "forward" : "reverse");
			break;
		}
		case SLAVE_CMD_SET_MOTOR_2_DIR:
		{
			uint8_t t = mSlavePacket.get8();
			DEBUG_OUTF("SLAVE_CMD_SET_MOTOR_2_DIR t=%s\r\n",t ? "forward" : "reverse");
			break;
		}
		case SLAVE_CMD_TOGGLE_MOTOR_1:
		{
			uint8_t t = mSlavePacket.get8();
			DEBUG_OUTF("SLAVE_CMD_TOGGLE_MOTOR_1 t=%s,%s\r\n",t&1 ? "on" : "off",t&2 ? "forward" : "reverse");
			break;
			
		}
		case SLAVE_CMD_TOGGLE_MOTOR_2:
		{
			uint8_t t = mSlavePacket.get8();
			DEBUG_OUTF("SLAVE_CMD_TOGGLE_MOTOR_2 t=%s,%s\r\n",t&1 ? "on" : "off",t&2 ? "forward" : "reverse");
			break;
			
		}
		case SLAVE_CMD_TOGGLE_FAN:
		{
			uint8_t t = mSlavePacket.get8();
			DEBUG_OUTF("SLAVE_CMD_TOGGLE_FAN t=%s\r\n",t ? "on" : "off");
			break;
		}
		case SLAVE_CMD_READ_FROM_EEPROM:
		{
			uint16_t offset = mSlavePacket.get16();
			uint8_t count = mSlavePacket.get8();
			DEBUG_OUTF("SLAVE_CMD_READ_FROM_EEPROM offset=%d, count=%d\r\n",offset,count);
			for (uint8_t i=0;i<count;i++)
				mSlaveReplyPacket.put8(0);
			break;
		}
		case SLAVE_CMD_WRITE_TO_EEPROM:
		{
			uint16_t offset = mSlavePacket.get16();
			uint8_t count = mSlavePacket.get8();
			DEBUG_OUTF("SLAVE_CMD_WRITE_TO_EEPROM offset=%d, count=%d\r\n",offset,count);
			mSlaveReplyPacket.put8(count);
			break;
		}

	}	

	copyPacketTo(mSlaveReplyPacket,mReplyPacket);
	return true;
#endif
}

bool HostComm::sendSlaveQuery(SlaveCommand cmd)
{
	mSlavePacket.reset();
	mSlavePacket.put8(0); //tool
	mSlavePacket.put8(cmd);
	sendSlavePacket();
	return readSlaveReply();
}

bool HostComm::readSlaveReply()
{
	mSlaveReplyPacket.reset();
	mSlaveState = WAIT_PACKET;
	uint8_t packetLen;
	uint8_t crc = 0;
	uint32_t curTime = millis();
	while(true)
	{
		if (rs485.available())
		{
			uint8_t b = rs485.receive();
			switch(mSlaveState)
			{
				case WAIT_PACKET:
					if (b == PACKET_START_BYTE)
						mSlaveState = PACKET_LEN;
					break;
				case PACKET_LEN:
					packetLen = b;
					mSlaveState = PACKET_PAYLOAD;
					break;
				case PACKET_PAYLOAD:
					crc = _crc_ibutton_update(crc,b);
					mSlaveReplyPacket.put8(b);
					packetLen--;
					if (!packetLen)
						mSlaveState = PACKET_CRC;
					break;
				case PACKET_CRC:
					if (b != crc)
						return false;
					return true;
			}
			curTime = millis();
		}
		
		
		if (millis() > curTime+SLAVE_TIMEOUT)
		{
			DEBUG_OUT("SLAVE TIMEOUT\r\n");
			return false;
		}
	}
	
}

void HostComm::updateSlaveWaitState()
{
	if (millis() > mLastSlaveReadyCheck+100)
	{
		if (sendSlaveQuery(SLAVE_CMD_IS_TOOL_READY))
		{
			uint8_t t = mSlaveReplyPacket.get8();
			if (t)
			{
				mainState = STATE_RUNNING;
				return;
			}
			else
				mLastSlaveReadyCheck = millis()+100;
		}
	}
	
	if (millis() > mSlaveWaitTimeout)
		mainState = STATE_RUNNING;
}

