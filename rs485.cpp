
#include "config.h"
#include "rs485.h"
#include "portAccess.h"
#include "uart.h"

#ifdef USE_RS485


RS485 rs485(RS485_UART,RS485_BAUDRATE);

RS485::RS485(uint8_t uartNum,uint16_t baudRate)
: UART(uartNum,baudRate)
{
	DDR(RS485_PORT_IE) |= _BV(RS485_PIN_IE);
	DDR(RS485_PORT_OE) |= _BV(RS485_PIN_OE);
	
}

void RS485::send(const uint8_t* buf,uint8_t size)
{
	PIN(RS485_PORT_OE) |= _BV(RS485_PIN_OE);
	UART::send(buf,size);
	PIN(RS485_PORT_OE) &= ~_BV(RS485_PIN_OE);
}

#endif