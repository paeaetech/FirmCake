#ifndef RINGBUFFER_H_MMYFI725
#define RINGBUFFER_H_MMYFI725

#include <inttypes.h>


//NOTE: buffer size must be ^2:  4,8,16,32,64,128,256 etc
class RingBuffer
{
public:
	RingBuffer(uint8_t* buf,uint16_t size)
	: mBuffer(buf),mBufferSize(size),mReadPos(0),mWritePos(0),mBytesAvailable(0)
	{
	}
	
	uint16_t available() const { return mBytesAvailable; }
	void put(uint8_t b) {
		if (available() < mBufferSize)
		{
			mBuffer[mWritePos]=b;
			mWritePos = (mWritePos+1) & (mBufferSize-1);
			mBytesAvailable++;
		}
	}
	uint8_t get() {
		uint8_t b = 0;
		if (available() > 0)
		{
			b = mBuffer[mReadPos];
			mReadPos = (mReadPos+1) & (mBufferSize-1);
			mBytesAvailable--;
			
		}
		return b;
	}
	
	uint16_t getSize() const { return mBufferSize; }
	
	void reset() { mReadPos = 0; mWritePos = 0; mBytesAvailable = 0; }
	
protected:
	uint8_t* mBuffer;
	uint16_t mBufferSize;
	
	uint16_t mReadPos;
	uint16_t mWritePos;
	uint16_t mBytesAvailable;
		
};



#endif /* end of include guard: RINGBUFFER_H_MMYFI725 */