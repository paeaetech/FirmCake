

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "config.h"
#include "version.h"
#include "portAccess.h"

#include "HostComm.h"
#include "uart.h"
#include "rs485.h"
#include "Timer.h"
#include "StepperController.h"
#include "psu.h"
#include "mmc/mmc.h"
#include "utils.h"

void setup()
{
#ifdef USE_PSU
	psu_init();
	psu_on();
#endif
	clock_init();
	
#ifdef USE_SDCARD
	mmcInit();
#endif

#ifdef USE_STEPPERS
//	stepperController.setPoint(0,0,0);
#endif

}

void loop()
{
	hostComm.update();
#ifdef USE_STEPPERS
	stepperController.update();
#endif

}


void main() __attribute__ ((noreturn));

void main() 
{
	setup();
	
	while(true)
	{
		loop();
	}
	
}
