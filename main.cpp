

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include "config.h"
#include "version.h"
#include "portAccess.h"
#include "State.h"
#include "eepromConfig.h"
#include "HostComm.h"
#include "uart.h"
#include "rs485.h"
#include "Timer.h"
#include "StepperController.h"
#include "psu.h"
#include "mmc/mmc.h"
#include "utils.h"
#include "fpmath.h"

const char spBuildId[] PROGMEM = BUILD_ID;
const char FIRMWARE_NAME[] PROGMEM = NAME " " VERSIONSTRING " " BUILD_ID "\r\n";

void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));

void wdt_init()
{
	wdt_reset();
	wdt_disable();
	return;
}


void initSystem()
{
	stepperController.setPoint(0,0,0);
}



void setup()
{
	DDR(LED_PORT) |= _BV(LED_PIN);
	
	clock_init();

	fpinit();

	LED_ON();
#ifdef USE_SDCARD
	mmcInit();
#endif
	
	initSystem();
	if (!eepromConfigValid())
	{
		eepromWriteDefaults();
	}
/*	
	uint8_t b;
	uint8_t p=0;
	while((b=pgm_read_byte(FIRMWARE_NAME+p)))
	{
		DEBUG_OUTB(b);
		p++;
	}
*/
#ifdef USE_PSU
	psu_init();
#endif
	LED_OFF();
	
	sei();
}



void loop()
{

	hostComm.update();

	switch(mainState)
	{
		case STATE_RUNNING:
			hostComm.processCommandBuffer();
			break;
		case STATE_DELAY:
			updateDelayState();
			break;
		case STATE_WAIT_FOR_SLAVE:
			hostComm.updateSlaveWaitState();
			break;
	}

	stepperController.update();

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
