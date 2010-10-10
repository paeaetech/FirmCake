
#include <avr/wdt.h>
#include "utils.h"
#include "Timer.h"
#include "config.h"
#include "uart.h"

#ifdef DEBUG
UART uart3(3,57600);
#endif

namespace {
#ifdef CLOCK_TIMER_8BIT
	Timer8 clockTimer(CLOCK_TIMER,1000);
#else
	Timer clockTimer(CLOCK_TIMER,1000);
#endif
	volatile uint32_t curMillis;
	
	void clockCallback(void* udata)
	{
		curMillis++;
	}
}

uint32_t led_activity;

void led_activity_on()
{
	led_activity=millis();
	_LED_ON();
}

void led_activity_off()
{
	led_activity=millis();
	_LED_OFF();
}

void led_update()
{
	if (millis() > led_activity+2000)
	{
		if (millis() % 5000 < 500)
			_LED_ON();
		else
			_LED_OFF();
	}
}

void clock_callback()
{
	curMillis++;
}

void clock_init()
{
	clockTimer.setCallback(clockCallback);
	clockTimer.enable();
}


void reset()
{
	wdt_enable(WDTO_1S);
	wdt_reset();
	while(true) {}
}



uint32_t millis()
{
	return curMillis;
}

