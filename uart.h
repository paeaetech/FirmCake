#ifndef UART_H_Y7TMN3TO
#define UART_H_Y7TMN3TO

#include <inttypes.h>
#include "RingBuffer.h"
//NOTE: buffer size must be ^2, 4,8,16,32,64, etc
#define UART_RECEIVE_BUFFER 32

class UART
{
public:
	UART(uint8_t uartNum,uint16_t baud);
	
	uint8_t available() { return mRingBuffer.available(); }
	uint8_t receive();
	virtual void send(uint8_t b);
	virtual void send(const uint8_t* buf,uint8_t size);

	void isrCallback();
private:
	
	uint8_t mBuffer[UART_RECEIVE_BUFFER];
	RingBuffer mRingBuffer;
	
	uint16_t mBaudrate;
	uint8_t mUartNum;
	
};

//atleast the first uart is always initialized
extern UART uart0;

#endif /* end of include guard: UART_H_Y7TMN3TO */
