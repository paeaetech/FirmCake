

#include "eepromConfig.h"
#include "eeprom.h"


uint8_t eepromConfigValid()
{
	return eeprom_read(0) == EEPROM_CONFIG_START_BYTE && eeprom_read(1) == EEPROM_CONFIG_END_BYTE;
	
}

void eepromWriteDefaults()
{
	eeprom_write(0,EEPROM_CONFIG_START_BYTE);
	eeprom_write(1,EEPROM_CONFIG_END_BYTE);
}

