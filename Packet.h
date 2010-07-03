#ifndef PACKET_H_9GVXJVZJ
#define PACKET_H_9GVXJVZJ

#include <inttypes.h>
#include "RingBuffer.h"

class Packet
{
public:
	Packet(uint8_t* buffer,uint16_t size) : mBuffer(buffer,size),mCommand(0) {}
	
	virtual void reset() { mCommand=0; mBuffer.reset(); }
	virtual void setCommand(uint8_t c) { mCommand = c; }
	virtual uint8_t getCommand() const { return mCommand; }
	virtual void processData(uint8_t b)
	{
		mBuffer.put(b);
	}
	
	uint16_t getBytesRemaining() { return mBuffer.available(); }
	
	uint8_t get8() 
	{
		return mBuffer.get();
	}
	uint16_t get16()
	{
		return ((uint16_t)get8()) << 8 + get8();
	}
	
	uint32_t get32()
	{
		return ((uint32_t)get16()) << 16 + get16();
	}
	
	
protected:
	
	RingBuffer mBuffer;
	uint8_t mCommand;
	
};



#endif /* end of include guard: PACKET_H_9GVXJVZJ */
