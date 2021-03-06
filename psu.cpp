

#include "psu.h"
#include "portAccess.h"

#ifdef USE_PSU
void psu_init()
{
	DDR(PSU_PORT) = _BV(PSU_PIN);
	psu_on();
}

void psu_on()
{
	PORT(PSU_PORT) &= ~_BV(PSU_PIN);
}

void psu_off()
{
	PORT(PSU_PORT) |= _BV(PSU_PIN);
}


#endif
