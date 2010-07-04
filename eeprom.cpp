
#include <avr/io.h>
#include <avr/eeprom.h>
#include "config.h"
#include "eeprom.h"



uint8_t eeprom_read(uint16_t addr)
{
	while(EECR & (1<<EEPE)) {}
	
	EEARH = addr>>8;
	EEARL = addr&0xff;
	EECR |= _BV(EERE);
	
	return EEDR;
}

void eeprom_write(uint16_t addr,uint8_t b)
{
	
	if (eeprom_read(addr) != b)
	{
		EEARH = addr>>8;
		EEARL = addr&0xff;
		EEDR = b;
		EECR |= _BV(EEMPE);
		EECR |= _BV(EEPE);
	}
	
}


