

#include <avr/io.h>
#include "config.h"
#include "version.h"
#include "portAccess.h"

void setup()
{
	PORT(PORTA) = 1;
	PIN(PORTA) = 1;
	DDR(PORTA) = 1;
	
	
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