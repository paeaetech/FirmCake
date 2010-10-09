

#include "fpmath.h"
#include <math.h>
#include "eeprom.h"

#define TABLE_SIZE 256


#define EEPROM_TABLE_TAG1 0xbe
#define EEPROM_TABLE_TAG2 0xba

#define EEPROM_TABLE_POS 2048
#define EEPROM_FINE_TABLE_POS(i) (EEPROM_TABLE_POS+i)

int16_t readtable(uint16_t pos)
{
	return eeprom_read(EEPROM_TABLE_POS+(pos<<1)+0) + (((int16_t)eeprom_read(EEPROM_TABLE_POS+(pos<<1)+1))<<8);
}

void writetable(uint16_t pos,int16_t v)
{
	eeprom_write(EEPROM_TABLE_POS+(pos<<1)+0,v&0xff);
	eeprom_write(EEPROM_TABLE_POS+(pos<<1)+1,v>>8);
}


void fpinit()
{
	if (eeprom_read(EEPROM_TABLE_POS-2) == EEPROM_TABLE_TAG1 && eeprom_read(EEPROM_TABLE_POS-1) == EEPROM_TABLE_TAG2)
		return;
		
	
	for (int i=0;i<TABLE_SIZE;i++)
	{
		float f = sin(((float)i/TABLE_SIZE)*3.1416f);
		int16_t v = (((int16_t)floor(f))<<8) + (uint8_t)((f-floor(f))*256);
		writetable(i,v);
	}
	
	for (int i=0;i<TABLE_SIZE;i++)
	{
		float f = sin(i/1.f);
		int16_t v = (((int16_t)floor(f))<<8) + (uint8_t)((f-floor(f))*256);
		writetable(EEPROM_FINE_TABLE_POS(i),v);
	}
		
	eeprom_write(EEPROM_TABLE_POS-2,EEPROM_TABLE_TAG1);
	eeprom_write(EEPROM_TABLE_POS-1,EEPROM_TABLE_TAG2);
	
}


#define SIN(x) (readtable(r>>8))
#define COS(x) (readtable((r>>8)+TABLE_SIZE/4))
#define SINFINE(x) (readtable(EEPROM_FINE_TABLE_POS(r>>8)))
#define COSFINE(x) (readtable(EEPROM_FINE_TABLE_POS((r>>8)+TABLE_SIZE/4)))


int16_t fpsin(int16_t r)
{
	return SIN(r)*COSFINE(r) + COS(r)*SINFINE(r);
}

int16_t fpcos(int16_t r)
{
	return COS(r)*COSFINE(r) - SIN(r)*SINFINE(r);
}


