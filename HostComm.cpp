
#include <util/crc16.h>
#include "config.h"
#include "HostComm.h"
#include "uart.h"
#include "Packet.h"
#include "StepperController.h"
#include "rs485.h"
#include "utils.h"
#include "version.h"
#include "owntypes.h"

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
		uint8_t b = uart0.receive();
		lastMillis = millis();
		switch(mState)
		{
			case WAIT_PACKET:
				if (b == PACKET_START_BYTE)
				{
					mState=PACKET_LEN;
					mPacket.reset();
					mReplyPacket.reset();
					mReplyPacket.setCommand(HOST_REPLY_OK);
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
					mState = PACKET_CRC;
				break;
			case PACKET_CRC:
				if (mCrc != b)
					sendReply(HOST_REPLY_INVALID_CRC);
				else
					processPacket();
				mState = WAIT_PACKET;
				break;
		}
	}
}

void HostComm::copyPacketTo(Packet& rPacket,Packet &rDestPacket)
{
	rDestPacket.reset();
	while(rPacket.getBytesRemaining())
	{
		rDestPacket.put8(rPacket.get8());
	}
}

void HostComm::processPacket()
{
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
			sendReply(HOST_REPLY_BUFFER_OVERFLOW);
	}
	else
	{
		switch((HostCommand)mPacket.getCommand())
		{
			case HOST_CMD_VERSION:
				mReplyPacket.put16(VERSION);
				break;
			case HOST_CMD_INIT:
				break;
			case HOST_CMD_GET_BUFFER_SIZE:
				mReplyPacket.put16(mCommandBuffer.getSpaceAvailable());
				break;
			case HOST_CMD_CLEAR_BUFFER:
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
			case HOST_CMD_RESET:
				reset();
				break;

			case HOST_CMD_GET_RANGE:
			case HOST_CMD_SET_RANGE:
			case HOST_CMD_ABORT:
			case HOST_CMD_PAUSE:
			case HOST_CMD_PROBE:
			case HOST_CMD_TOOL_QUERY:
			case HOST_CMD_IS_FINISHED:
			case HOST_CMD_READ_EEPROM:
			case HOST_CMD_WRITE_EEPROM:
			
		  	case HOST_CMD_CAPTURE_TO_FILE:
			case HOST_CMD_END_CAPTURE:
		  	case HOST_CMD_PLAYBACK_CAPTURE:
			case HOST_CMD_NEXT_FILENAME:
			case HOST_CMD_GET_DBG_REG:
			default:
				mReplyPacket.setCommand(HOST_REPLY_UNSUPPORTED);
				break;
		}
		sendReply();
	}
}

void HostComm::processCommandBuffer()
{
	if (mCommandBuffer.available())
	{
	 	HostCommand cmd = (HostCommand)mCommandBuffer.get();
		
		switch(cmd)
		{
			case HOST_CMD_TOOL_COMMAND:
			{
				
			}
			default:
				break;
		}
		
	}
}


void HostComm::sendReply()
{
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
}

void HostComm::sendReply(HostReply r)
{
	uart0.send(PACKET_START_BYTE);
	uart0.send(1); //length
	uart0.send((uint8_t)r);
	uart0.send(_crc_ibutton_update(0,r));
}


