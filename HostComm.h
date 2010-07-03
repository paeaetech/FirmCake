#ifndef HOSTCOMM_H_9VL69MSF
#define HOSTCOMM_H_9VL69MSF

#include <inttypes.h>
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
	bool hasFullPacket() { return false; }
	
	Packet& getPacket() { return mPacket; }
	Packet& getReplyPacket() { return mReplyPacket; }
	
protected:
	
	void processPacket();
	void sendReply(HostReply r);
	
	uint8_t mReceiveBuffer[HOST_RECEIVE_BUFFER_SIZE];
	uint8_t mReplyBuffer[HOST_REPLY_BUFFER_SIZE];
	
	Packet mPacket;
	Packet mReplyPacket;
	
	HostState mState;
	uint8_t mCrc;
	uint16_t mPacketLen;
	uint32_t lastMillis;
	
	uint32_t mCommandsProcessed;
};

extern HostComm hostComm;


#endif /* end of include guard: HOSTCOMM_H_9VL69MSF */
