#ifndef EEPROM_H
#define EEPROM_H
#include "main.h"

uint8_t eeprom_read(uint8_t addr);
void eeprom_write(uint8_t addr, uint8_t data);
#endif


