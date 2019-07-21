#include "Arduino.h"
#include "SoftWire.h"

// initialize I2C
int VL53L0X_i2c_init(SoftWire *i2c);
int VL53L0X_write_multi(uint8_t deviceAddress, uint8_t index, uint8_t *pdata, uint32_t count, SoftWire *i2c);
int VL53L0X_read_multi(uint8_t deviceAddress, uint8_t index, uint8_t *pdata, uint32_t count, SoftWire *i2c);
int VL53L0X_write_byte(uint8_t deviceAddress, uint8_t index, uint8_t data, SoftWire *i2c);
int VL53L0X_write_word(uint8_t deviceAddress, uint8_t index, uint16_t data, SoftWire *i2c);
int VL53L0X_write_dword(uint8_t deviceAddress, uint8_t index, uint32_t data, SoftWire *i2c);
int VL53L0X_read_byte(uint8_t deviceAddress, uint8_t index, uint8_t *data, SoftWire *i2c);
int VL53L0X_read_word(uint8_t deviceAddress, uint8_t index, uint16_t *data, SoftWire *i2c);
int VL53L0X_read_dword(uint8_t deviceAddress, uint8_t index, uint32_t *data, SoftWire *i2c);
