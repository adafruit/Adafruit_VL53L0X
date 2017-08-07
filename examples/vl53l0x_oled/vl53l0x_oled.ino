/* This example shows how to use continuous mode to take
range measurements with the VL53L0X. It is based on
vl53l0x_ContinuousRanging_Example.c from the VL53L0X API.

The range readings are in units of mm. */

#include <Wire.h>
#include <VL53L0X.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306();

VL53L0X sensor;

#if (SSD1306_LCDHEIGHT != 32)
 #error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()
{
  Serial.begin(9600);
    
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  display.display();
  delay(1000);
    
  
  Wire.begin();

  sensor.init();
  sensor.setTimeout(500);

  // Start continuous back-to-back mode (take readings as
  // fast as possible).  To use continuous timed mode
  // instead, provide a desired inter-measurement period in
  // ms (e.g. sensor.startContinuous(100)).
  sensor.startContinuous();

  // text display big!
  display.setTextSize(4);
  display.setTextColor(WHITE);
}

void loop()
{
  uint16_t reading = sensor.readRangeContinuousMillimeters();

  if (reading > 8000) {
    display.display();
    display.clearDisplay();
    return;
  }
  
  Serial.print(sensor.readRangeContinuousMillimeters());
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  display.clearDisplay();
  display.setCursor(0,0);
  display.print(reading);
  display.print("mm");
  display.display();
  Serial.println();
  delay(50);
}
