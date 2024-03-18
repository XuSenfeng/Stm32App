#include "i2c/eeprom.h"
#include "i2c/i2c_hal.h"

uint8_t eeprom_read(uint8_t addr){
	uint8_t data;
	I2CStart();
	I2CSendByte(EEPROM_ADDRESS_WRITE);
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(EEPROM_ADDRESS_READ);
	I2CWaitAck();
	data = I2CReceiveByte();
	I2CSendNotAck();
	I2CStop();
	return data;
}

void eeprom_write(uint8_t addr, uint8_t data){

	I2CStart();
	I2CSendByte(EEPROM_ADDRESS_WRITE);
	I2CWaitAck();
	I2CSendByte(addr);
	I2CWaitAck();
	I2CSendByte(data);
	I2CWaitAck();	
	I2CStop();
}
