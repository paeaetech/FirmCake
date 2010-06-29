
#include <avr/io.h>
#include <avr/eeprom.h>
#include "config.h"
#include "eeprom.h"



uint8_t eeprom_read(uint8_t addr)
{
	while(EECR & (1<<EEPE)) {}
	
	EEAR = addr;
	EECR |= _BV(EERE);
	
	return EEDR;
}

void eeprom_write(uint8_t addr,uint8_t b)
{
	
	if (eeprom_read(addr) != b)
	{
		EEAR = addr;
		EEDR = b;
		EECR |= _BV(EEMPE);
		EECR |= _BV(EEPE);
	}
	
}


