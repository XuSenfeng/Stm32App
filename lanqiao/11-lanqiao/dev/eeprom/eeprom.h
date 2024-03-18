#ifndef EEPROM_H
#define EEPROM_H
#include "main.h"

uint8_t eeprom_read(uint8_t addr);
void eeprom_write(uint8_t addr, uint8_t data);
void eeprom_write32(uint8_t addr, uint32_t data);
uint32_t eeprom_read32(uint8_t addr);
#endif


