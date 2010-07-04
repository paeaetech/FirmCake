
#include <avr/wdt.h>
#include "utils.h"
#include "Timer.h"
#include "config.h"
#include "uart.h"

#ifdef DEBUG
UART uart3(3,57600);
#endif

namespace {
#if !defined(__AVR_ATmega644P__) //sanguino
	Timer clockTimer(CLOCK_TIMER,1000);
#endif
	volatile uint32_t curMillis;
	
	void clockCallback(void* udata)
	{
		curMillis++;
	}
}

void clock_callback()
{
	curMillis++;
}

void clock_init()
{
#if !defined(__AVR_ATmega644P__) //sanguino
	clockTimer.setCallback(clockCallback);
	clockTimer.enable();
#endif
}


void reset()
{
	wdt_enable(WDTO_1S);
}



uint32_t millis()
{
	return curMillis;
}

