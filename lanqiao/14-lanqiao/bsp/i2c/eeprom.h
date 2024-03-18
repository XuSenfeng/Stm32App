#ifndef EEPROM_H
#define EEPROM_H
#include "main.h"
#define EEPROM_ADDRESS_READ 		0xa1
#define EEPROM_ADDRESS_WRITE 		0xa0


uint8_t eeprom_read(uint8_t addr);
void eeprom_write(uint8_t addr, uint8_t data);
#endif /*EEPROM_H*/
