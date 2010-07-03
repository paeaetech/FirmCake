
#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include "uart.h"
#include "portAccess.h"

#ifndef F_CPU
#error F_CPU not defined
#endif

#if !defined(__AVR_ATmega1280__)
#error UART doesnt support current cpu
#endif

UART uart0(0,HOST_BAUDRATE);

namespace {
	
	uint16_t calcUBRR(uint16_t baud)
	{
		return F_CPU/16/baud-1;
	}	


	volatile uint8_t* portUDR[4] = { &UDR0, &UDR1, &UDR2, &UDR3 };
	volatile uint8_t* portDDR[4] = { &DDRE, &DDRD, &DDRH, &DDRJ };
	uint8_t portRxPin[4] = { _BV(PE1), _BV(PD3), _BV(PH1), _BV(PJ1) };
	
	UART* uartInstance[4];
}


ISR(USART0_RX_vect)
{
	uartInstance[0]->isrCallback();
}

ISR(USART1_RX_vect)
{
	uartInstance[1]->isrCallback();
}

ISR(USART2_RX_vect)
{
	uartInstance[2]->isrCallback();
}

ISR(USART3_RX_vect)
{
	uartInstance[3]->isrCallback();
}



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


UART::UART(uint8_t uartNum,uint16_t baudRate)
 : mRingBuffer(mBuffer,UART_RECEIVE_BUFFER),
   mUartNum(uartNum), 
   mBaudrate(baudRate)
{
	uartInstance[uartNum] = this;
	uint16_t ubrr = calcUBRR(mBaudrate);
	UBRRnH = ubrr>>8;
	UBRRnL = ubrr;
	UCSRnB = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE);
	UCSRnC = _BV(UCSZ1) | _BV(UCSZ0);
	
	//set tx pin as output
	*portDDR[uartNum] |= portRxPin[uartNum];
}

void UART::send(uint8_t b)
{
	WAIT_SEND;
	
	UDRn = b;
}

void UART::send(const uint8_t* b,uint8_t size)
{
	while(size--)
	{
		send(*b++);
	}
}

uint8_t UART::receive()
{
	return mRingBuffer.get();
}

void UART::isrCallback()
{
	mRingBuffer.put(UDRn);
}


