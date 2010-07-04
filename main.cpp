

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
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

const char spBuildId[] PROGMEM = BUILD_ID;

void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));

void wdt_init()
{
	wdt_reset();
	wdt_disable();
	return;
}


void initSystem()
{
#ifdef USE_STEPPERS
	stepperController.setPoint(0,0,0);
#endif
}

void setup()
{
	DDR(DEBUG_PORT) |= _BV(DEBUG_PIN);
	DEBUG_ON();
	
#ifdef USE_PSU
	psu_init();
#endif
	clock_init();
	
#ifdef USE_SDCARD
	mmcInit();
#endif
	
	initSystem();
	DEBUG_OFF();
	DEBUG_OUT(NAME " " VERSIONSTRING " ");
	DEBUG_OUT(BUILD_ID);
	sei();
}


void loop()
{
	hostComm.update();
	hostComm.processCommandBuffer();
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
