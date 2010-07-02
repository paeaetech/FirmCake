

#include "psu.h"
#include "portAccess.h"

#ifdef USE_PSU
void psu_init()
{
	DDR(PSU_PORT) = _BV(PSU_PIN);
	psu_off();
}

void psu_on()
{
	PIN(PSU_PORT) &= ~_BV(PSU_PIN);
}

void psu_off()
{
	PIN(PSU_PORT) |= _BV(PSU_PIN);
}


#endif