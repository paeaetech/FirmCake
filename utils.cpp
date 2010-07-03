
#include "utils.h"
#include "Timer.h"
#include "config.h"

namespace {
	Timer clockTimer(CLOCK_TIMER,1000);
	volatile uint32_t curMillis;
	
	void clockCallback(void* udata)
	{
		curMillis++;
	}
}

void clock_init()
{
	clockTimer.setCallback(clockCallback);
	clockTimer.enable();
}




uint32_t millis()
{
	return curMillis;
}

