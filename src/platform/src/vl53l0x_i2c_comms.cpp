#include "vl53l0x_i2c_platform.h"
#include "vl53l0x_def.h"

//#define I2C_DEBUG

int VL53L0X_i2c_init(void) {
  Wire.begin();
  return VL53L0X_ERROR_NONE;
}

int VL53L0X_write_multi(uint8_t deviceAddress, uint8_t index, uint8_t *pdata, uint32_t count) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(index);
#ifdef I2C_DEBUG
  Serial.print("\tWriting "); Serial.print(count); Serial.print(" to addr 0x"); Serial.print(index, HEX); Serial.print(": ");
#endif
  while(count--) {
    Wire.write((uint8_t)pdata[0]);
#ifdef I2C_DEBUG
    Serial.print("0x"); Serial.print(pdata[0], HEX); Serial.print(", ");
#endif
    pdata++;
  }
#ifdef I2C_DEBUG
  Serial.println();
#endif
  Wire.endTransmission();
  return VL53L0X_ERROR_NONE;
}

int VL53L0X_read_multi(uint8_t deviceAddress, uint8_t index, uint8_t *pdata, uint32_t count) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(index);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (byte)count);
#ifdef I2C_DEBUG
  Serial.print("\tReading "); Serial.print(count); Serial.print(" from addr 0x"); Serial.print(index, HEX); Serial.print(": ");
#endif

  while (count--) {
    pdata[0] = Wire.read();
#ifdef I2C_DEBUG
    Serial.print("0x"); Serial.print(pdata[0], HEX); Serial.print(", ");
#endif
    pdata++;
  }
#ifdef I2C_DEBUG
  Serial.println();
#endif
  return VL53L0X_ERROR_NONE;
}

int VL53L0X_write_byte(uint8_t deviceAddress, uint8_t index, uint8_t data) {
  return VL53L0X_write_multi(deviceAddress, index, &data, 1);
}

int VL53L0X_write_word(uint8_t deviceAddress, uint8_t index, uint16_t data) {
  uint8_t buff[2];
  buff[1] = data & 0xFF;
  buff[0] = data >> 8;
  return VL53L0X_write_multi(deviceAddress, index, buff, 2);
}

int VL53L0X_write_dword(uint8_t deviceAddress, uint8_t index, uint32_t data) {
  uint8_t buff[4];

  buff[3] = data & 0xFF;
  buff[2] = data >> 8;
  buff[1] = data >> 16;
  buff[0] = data >> 24;

  return VL53L0X_write_multi(deviceAddress, index, buff, 4);
}

int VL53L0X_read_byte(uint8_t deviceAddress, uint8_t index, uint8_t *data) {
  return VL53L0X_read_multi(deviceAddress, index, data, 1);
}

int VL53L0X_read_word(uint8_t deviceAddress, uint8_t index, uint16_t *data) {
  uint8_t buff[2];
  int r = VL53L0X_read_multi(deviceAddress, index, buff, 2);

  uint16_t tmp;
  tmp = buff[0];
  tmp <<= 8;
  tmp |= buff[1];
  *data = tmp;

  return r;
}

int VL53L0X_read_dword(uint8_t deviceAddress, uint8_t index, uint32_t *data) {
  uint8_t buff[4];
  int r = VL53L0X_read_multi(deviceAddress, index, buff, 4);

  uint32_t tmp;
  tmp = buff[0];
  tmp <<= 8;
  tmp |= buff[1];
  tmp <<= 8;
  tmp |= buff[2];
  tmp <<= 8;
  tmp |= buff[3];

  *data = tmp;

  return r;
}
