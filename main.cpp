

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "config.h"
#include "version.h"
#include "portAccess.h"
#include "uart.h"

#include "rs485.h"
#include "Timer.h"
#include "StepperController.h"
#include "psu.h"

void setup()
{
#ifdef USE_PSU
	psu_init();
	psu_on();
#endif

#ifdef USE_STEPPERS
//	stepperController.setPoint(0,0,0);
#endif

}

void loop()
{
#ifdef USE_STEPPERS
//	stepperController.update();
#endif

}



int main()
{
	setup();
	
	while(true)
	{
		loop();
	}
	
}