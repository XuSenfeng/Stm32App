#include "eeprom/eeprom.h"
#include "eeprom/i2c_hal.h"
/*
void I2CStart(void);
void I2CStop(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(void);
void I2CSendNotAck(void);
void I2CSendByte(unsigned char cSendByte);
unsigned char I2CReceiveByte(void);
void I2CInit(void);
*/
#define EEPROM_ADDRESS_READ 		0xa1
#define EEPROM_ADDRESS_WRITE 		0xa0
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
