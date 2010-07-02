#ifndef UART_H_Y7TMN3TO
#define UART_H_Y7TMN3TO

#include <inttypes.h>

//NOTE: buffer size must be ^2, 4,8,16,32,64, etc
#define UART_RECEIVE_BUFFER 32

class UART
{
public:
	UART(uint8_t uartNum,uint16_t baud);
	
	uint8_t available();
	uint8_t receive();
	virtual void send(uint8_t b);
	virtual void send(const uint8_t* buf,uint8_t size);

	void isrCallback();
private:
	
	volatile uint8_t mReceiveBuffer[UART_RECEIVE_BUFFER];
	uint8_t mReadPos;
	volatile uint8_t mWritePos;
	volatile uint8_t mBytesAvailable;

	
	uint16_t mBaudrate;
	uint8_t mUartNum;
	
};



#endif /* end of include guard: UART_H_Y7TMN3TO */
