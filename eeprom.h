#ifndef EEPROM_H_4LS4U3EF
#define EEPROM_H_4LS4U3EF

#include <inttypes.h>


uint8_t eeprom_read(uint16_t addr);
void eeprom_write(uint16_t addr,uint8_t b);



#endif /* end of include guard: EEPROM_H_4LS4U3EF */
