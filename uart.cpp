
#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include "uart.h"
#include "portAccess.h"
#include "utils.h"
#ifndef F_CPU
#error F_CPU not defined
#endif

UART uart0(HOST_UART,HOST_BAUDRATE);

namespace {
	
	uint16_t calcUBRR(uint16_t baud)
	{
		return F_CPU/16/baud-1;
	}	

	
	UART* uartInstance[NUM_UARTS];
}

#if defined(__AVR_ATmega1280__) //mega
	volatile uint8_t* portUDR[NUM_UARTS] = { 
			&UDR0, 
			&UDR1,
			&UDR2,
			&UDR3
	};
	volatile uint8_t* portDDR[NUM_UARTS] = { 
			&DDRE, 
			&DDRD,
			&DDRH,
			&DDRJ
	};

	uint8_t portTxPin[NUM_UARTS] = { 
		_BV(PE1), 
		_BV(PD3), 
		_BV(PH1), 
		_BV(PJ1) 
	};
#elif defined(__AVR_ATmega644P__) //sanguino
	volatile uint8_t* portUDR[NUM_UARTS] = { 
			&UDR0, 
			&UDR1,
	};
	volatile uint8_t* portDDR[NUM_UARTS] = { 
			&DDRD, 
			&DDRD,
	};

	uint8_t portTxPin[NUM_UARTS] = { 
		_BV(PD1), 
		_BV(PD3), 
	};

#else
#error UART doesnt support current cpu
#endif




ISR(USART0_RX_vect)
{
	uartInstance[0]->isrCallback();
}
#if NUM_UARTS > 1
ISR(USART1_RX_vect)
{
	uartInstance[1]->isrCallback();
}
#endif
#if NUM_UARTS > 2
ISR(USART2_RX_vect)
{
	uartInstance[2]->isrCallback();
}
#endif
#if NUM_UARTS > 3
ISR(USART3_RX_vect)
{
	uartInstance[3]->isrCallback();
}
#endif



UART::UART(uint8_t uartNum,uint16_t baudRate)
 : mRingBuffer(mBuffer,UART_RECEIVE_BUFFER_SIZE),
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
	*portDDR[uartNum] |= portTxPin[uartNum];
	
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


