#ifndef HOSTCOMM_H_9VL69MSF
#define HOSTCOMM_H_9VL69MSF

#include <inttypes.h>
#include "RingBuffer.h"
#include "Packet.h"
#include "config.h"
#include "Commands.h"


#define PACKET_START_BYTE 0xD5

enum HostState
{
	WAIT_PACKET,
	PACKET_LEN,
	PACKET_PAYLOAD,
	PACKET_CRC,
};

class HostComm
{
public:
	HostComm();
	
	void update();
	void processCommandBuffer();

	bool hasFullPacket() { return false; }
	
	Packet& getPacket() { return mPacket; }
	Packet& getReplyPacket() { return mReplyPacket; }
	
	void updateSlaveWaitState();
	
protected:
	
	
	void processPacket();
	void sendReply(HostReply r);
	void sendReply();
	
	
	
	uint8_t mReceiveBuffer[HOST_RECEIVE_BUFFER_SIZE];
	uint8_t mReplyBuffer[HOST_REPLY_BUFFER_SIZE];
	uint8_t mSlaveBuffer[HOST_SLAVE_BUFFER_SIZE];
	uint8_t mSlaveReplyBuffer[HOST_SLAVE_REPLY_BUFFER_SIZE];
	uint8_t mCommandBuf[COMMAND_BUFFER_SIZE];
	
	void copyPacketTo(Packet& rPacket,Packet& rDestPacket);
	void packetFromCommandBuffer(Packet& rPacket);
	
	void sendSlavePacket();
	bool sendSlaveQuery();
	bool sendSlaveQuery(SlaveCommand cmd);
	
	bool readSlaveReply();
	
	Packet mPacket;
	Packet mReplyPacket;
	Packet mSlavePacket;
	Packet mSlaveReplyPacket;
	RingBuffer mCommandBuffer;
	
	HostState mState;
	HostState mSlaveState;
	uint32_t mLastSlaveReadyCheck;
	uint32_t mSlaveWaitTimeout;
	
	uint8_t mCrc;
	uint16_t mPacketLen;
	uint32_t lastMillis;
	
	uint32_t mCommandsProcessed;
};

extern HostComm hostComm;


#endif /* end of include guard: HOSTCOMM_H_9VL69MSF */
