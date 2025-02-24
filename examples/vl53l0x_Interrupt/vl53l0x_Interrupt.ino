#include "Adafruit_VL53L0X.h"
const byte VL53L0X_InterruptPin = 6;
const byte VL53L0X_ShutdownPin = 9;
volatile byte VL53L0X_State = LOW;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (!Serial) {
    delay(1);
  }
  Serial.println(F("VL53L0X API Interrupt Ranging example\n\n"));

  pinMode(VL53L0X_ShutdownPin, OUTPUT);
  
  pinMode(VL53L0X_ShutdownPin, INPUT_PULLUP);
  pinMode(VL53L0X_InterruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(VL53L0X_InterruptPin), VL53L0XISR,
                  CHANGE);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // if lox.begin fails its because it was a warm boot and the VL53L0X is in
  // continues measurement mode we can use an IO pin to reset the device in case
  // we get stuck in this mode
  while (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    Serial.println("Adafruit VL53L0X XShut set Low to Force HW Reset");
    digitalWrite(VL53L0X_ShutdownPin, LOW);
    delay(100);
    digitalWrite(VL53L0X_ShutdownPin, HIGH);
    Serial.println("Adafruit VL53L0X XShut set high to Allow Boot");
    delay(100);
  }

  // Second Parameter options are VL53L0X_GPIOFUNCTIONALITY_OFF,
  // VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_LOW,
  // VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_HIGH,
  // VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_OUT,
  // VL53L0X_GPIOFUNCTIONALITY_NEW_MEASURE_READY

  Serial.println("Set GPIO Config so if range is lower the LowThreshold "
                 "trigger Gpio Pin ");
  lox.setGpioConfig(VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,
                    VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_LOW,
                    VL53L0X_INTERRUPTPOLARITY_LOW);

  // Set Interrupt Thresholds
  // Low reading set to 50mm  High Set to 100mm
  FixPoint1616_t LowThreshold = (50 * 65536.0);
  FixPoint1616_t HighThreshold = (100 * 65536.0);
  Serial.println("Set Interrupt Thresholds... ");
  lox.setInterruptThresholds(LowThreshold, HighThreshold, true);

  // Enable Continuous Measurement Mode
  Serial.println("Set Mode VL53L0X_DEVICEMODE_CONTINUOUS_RANGING... ");
  lox.setDeviceMode(VL53L0X_DEVICEMODE_CONTINUOUS_RANGING, false);

  Serial.println("StartMeasurement... ");
  lox.startMeasurement();
}

void VL53L0XISR() {
  // Read if we are high or low
  VL53L0X_State = digitalRead(VL53L0X_InterruptPin);
  // set the built in LED to reflect in range on for Out of range off for in
  // range
  digitalWrite(LED_BUILTIN, VL53L0X_State);
}

void loop() {
  if (VL53L0X_State == LOW) {
    VL53L0X_RangingMeasurementData_t measure;
    Serial.print("Reading a measurement... ");
    lox.getRangingMeasurement(
        &measure, false); // pass in 'true' to get debug data printout!

    if (measure.RangeStatus != 4) { // phase failures have incorrect data
      Serial.print("Distance (mm): ");
      Serial.println(measure.RangeMilliMeter);
    } else {
      Serial.println(" out of range ");
    }
    // you have to clear the interrupt to get triggered again
    lox.clearInterruptMask(false);

  } else {
    delay(10);
  }
}
