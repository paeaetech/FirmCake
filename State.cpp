
#include "State.h"
#include "utils.h"

MachineState mainState;

namespace {
	
	uint32_t delayTime;
	
}

void updateDelayState()
{
	if (mainState == STATE_DELAY)
	{
		if (millis() >= delayTime)
			mainState=STATE_RUNNING;
	}
}

void initDelayState(uint32_t delayMs)
{
	delayTime = millis()+delayMs;
	mainState=STATE_DELAY;
}