

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "config.h"
#include "version.h"
#include "portAccess.h"
#include "uart.h"

#include "rs485.h"


void operator delete(void*) {}


void setup()
{
	rs485.send((const uint8_t*)"  ",2);
}

void loop()
{
	
}



int main()
{
	setup();
	
	while(true)
	{
		loop();
	}
	
}