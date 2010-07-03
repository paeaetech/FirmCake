
#include <util/crc16.h>
#include "config.h"
#include "HostComm.h"
#include "uart.h"
#include "Packet.h"
#include "StepperController.h"
#include "rs485.h"
#include "utils.h"

HostComm hostComm;

HostComm::HostComm()
: mPacket(mReceiveBuffer,HOST_RECEIVE_BUFFER_SIZE), mReplyPacket(mReplyBuffer,HOST_REPLY_BUFFER_SIZE),
  mCommandsProcessed(0), mState(WAIT_PACKET)
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

void HostComm::processPacket()
{
	mPacket.setCommand(mPacket.get8());
	switch((HostCommand)mPacket.getCommand())
	{
		
		default:
			break;
	}
}

void HostComm::sendReply(HostReply r)
{
	uart0.send(PACKET_START_BYTE);
	uart0.send(1); //length
	uart0.send((uint8_t)r);
	uart0.send(_crc_ibutton_update(0,r));
}


