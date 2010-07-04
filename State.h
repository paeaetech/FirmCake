#ifndef STATE_H_KJ7K2IVN
#define STATE_H_KJ7K2IVN

#include <inttypes.h>

enum MachineState
{
	STATE_RUNNING = 0,
	STATE_DELAY,
	STATE_WAIT_FOR_SLAVE,
};

extern MachineState mainState;

void updateDelayState();
void initDelayState(uint32_t delayMs);

#endif /* end of include guard: STATE_H_KJ7K2IVN */
