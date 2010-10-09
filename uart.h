#ifndef UART_H_Y7TMN3TO
#define UART_H_Y7TMN3TO

#include <inttypes.h>
#include "RingBuffer.h"
//NOTE: buffer size must be ^2, 4,8,16,32,64, etc
#define UART_RECEIVE_BUFFER_SIZE 32


extern volatile uint8_t* portUDR[NUM_UARTS];
extern volatile uint8_t* portDDR[NUM_UARTS];
extern uint8_t portTxPin[NUM_UARTS];



#define UDRn *(volatile uint8_t*)(portUDR[mUartNum])
#define UBRRnH *(volatile uint8_t*)((&UDRn)-1)
#define UBRRnL *(volatile uint8_t*)((&UDRn)-2)
#define UCSRnC *(volatile uint8_t*)((&UDRn)-4)
#define UCSRnB *(volatile uint8_t*)((&UDRn)-5)
#define UCSRnA *(volatile uint8_t*)((&UDRn)-6)

#define RXEN RXEN0
#define TXEN TXEN0
#define UDRE UDRE0
#define TXC TXC0
#define RXCIE RXCIE0
#define UCSZ2 UCSZ02
#define UCSZ1 UCSZ01
#define UCSZ0 UCSZ00

#define WAIT_SEND while(!(UCSRnA & _BV(UDRE))) {}


class UART
{
public:
	UART(uint8_t uartNum,uint16_t baud);
	
	uint8_t available() { return mRingBuffer.available(); }
	uint8_t receive();
	virtual void send(uint8_t b);
	virtual void send(const uint8_t* buf,uint8_t size);

	void isrCallback();
protected:
	
	uint8_t mBuffer[UART_RECEIVE_BUFFER_SIZE];
	RingBuffer mRingBuffer;
	
	uint16_t mBaudrate;
	uint8_t mUartNum;
	
};

//atleast the first uart is always initialized
extern UART uart0;

#endif /* end of include guard: UART_H_Y7TMN3TO */
