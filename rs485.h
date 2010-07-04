
#ifndef RS485_H_H86QACKG
#define RS485_H_H86QACKG

#include "uart.h"

class RS485 : public UART 
{
public:
	RS485(uint8_t uartNum,uint16_t baudRate);
	
	void start();
	void end();
	
	virtual void send(const uint8_t* buf,uint8_t size);
	virtual void send(uint8_t b) { UART::send(b); }
private:
		
};

extern RS485 rs485;


#endif /* end of include guard: RS485_H_H86QACKG */
