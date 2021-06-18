#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  lox.begin(0x29);// put any address between 0x29 to 0x7F
  Serial.begin(9600);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }

  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1);
  }
  // power
  Serial.println(F("VL53L0X API Simple Ranging example\n\n"));
}


void loop() {
  VL53L0X_RangingMeasurementData_t measure;

  Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): ");
    Serial.println(measure.RangeMilliMeter);

    //  for centimeter remove comments

    //    int centimeter;
    //    centimeter = measure.RangeMilliMeter / 10; // convetrint mm into Cm
    //    Serial.print("  , Distance (Cm): ");
    //    Serial.print(centimeter);
    //    Serial.println(" ");


  } else {
    Serial.println(" out of range ");
  }

  delay(200); //little delay to improve simulation performance
}
