
#ifndef RS485_H_H86QACKG
#define RS485_H_H86QACKG

#include "uart.h"

class RS485 : public UART 
{
public:
	RS485(uint8_t uartNum,uint16_t baudRate);
	~RS485() {}
	
	virtual void send(const uint8_t* buf,uint8_t size);
private:
	virtual void send(uint8_t b) { UART::send(b); }
		
};

#ifdef USE_RS485
extern RS485 rs485;
#endif


#endif /* end of include guard: RS485_H_H86QACKG */
