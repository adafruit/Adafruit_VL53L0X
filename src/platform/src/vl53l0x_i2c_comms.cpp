#include "vl53l0x_i2c_platform.h"
#include "vl53l0x_def.h"

int VL53L0X_i2c_init(void) {
  Wire.begin();
  Serial.println("wire init");
  return VL53L0X_ERROR_NONE;
}

int VL53L0X_write_multi(uint8_t deviceAddress, uint8_t index, uint8_t *pdata, uint32_t count) {
  Serial.print("Device 0x"); Serial.println(deviceAddress, HEX);
  Wire.beginTransmission(deviceAddress);
  Wire.write(index);
  Serial.print("writing "); Serial.print(count); 
  Serial.print(" to addr 0x"); Serial.print(index, HEX);
  Serial.print(": ");
  while(count--) {
    Wire.write((uint8_t)pdata[0]);
    Serial.print("0x"); Serial.print(pdata[0], HEX); Serial.print(", ");
    pdata++;
  }
  Serial.println();
  Wire.endTransmission();
  return VL53L0X_ERROR_NONE;
}

int VL53L0X_read_multi(uint8_t deviceAddress, uint8_t index, uint8_t *pdata, uint32_t count) {
  Serial.print("Device 0x"); Serial.println(deviceAddress, HEX);
  Wire.beginTransmission(deviceAddress);
  Wire.write(index);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (byte)count);
  Serial.print("reading "); Serial.print(count); 
  Serial.print(" from addr 0x"); Serial.print(index, HEX);
  Serial.print(": ");

  while (count--) {
    pdata[0] = Wire.read();
    Serial.print("0x"); Serial.print(pdata[0], HEX); Serial.print(", ");
    pdata++;
  }
  Serial.println();
  return VL53L0X_ERROR_NONE;
}

int VL53L0X_write_byte(uint8_t deviceAddress, uint8_t index, uint8_t data) {
  return VL53L0X_write_multi(deviceAddress, index, &data, 1);
}

int VL53L0X_write_word(uint8_t deviceAddress, uint8_t index, uint16_t data) {
  uint8_t buff[2];
  memcpy(buff, data, 2);
  return VL53L0X_write_multi(deviceAddress, index, buff, 2);
}

int VL53L0X_write_dword(uint8_t deviceAddress, uint8_t index, uint32_t data) {
  uint8_t buff[4];
  memcpy(buff, data, 4);
  return VL53L0X_write_multi(deviceAddress, index, buff, 4);
}

int VL53L0X_read_byte(uint8_t deviceAddress, uint8_t index, uint8_t *data) {
  return VL53L0X_read_multi(deviceAddress, index, data, 1);
}

int VL53L0X_read_word(uint8_t deviceAddress, uint8_t index, uint16_t *data) {
  uint8_t buffer[2];
  int r = VL53L0X_read_multi(deviceAddress, index, buffer, 2);
  memcpy(data, buffer, 2);
  return r;
}

int VL53L0X_read_dword(uint8_t deviceAddress, uint8_t index, uint32_t *data) {
  uint8_t buffer[4];
  int r = VL53L0X_read_multi(deviceAddress, index, buffer, 4);
  memcpy(data, buffer, 4);
  return r;
}
