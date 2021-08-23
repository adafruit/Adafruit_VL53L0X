#include "Adafruit_VL53L0X.h"
const byte VL53LOX_InterruptPin = 6;
const byte VL53LOX_ShutdownPin = 9;
volatile byte VL53LOX_State = LOW;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (!Serial) {
    delay(1);
  }
  Serial.println(F("VL53L0X API Interrupt Ranging example\n\n"));

  pinMode(VL53LOX_ShutdownPin, INPUT_PULLUP);
  pinMode(VL53LOX_InterruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(VL53LOX_InterruptPin), VL53LOXISR,
                  CHANGE);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // if lox.begin failes its becasue it was a warm boot and the VL53LOX is in
  // continues mesurement mode we can use an IO pin to reset the device in case
  // we get stuck in this mode
  while (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    Serial.println("Adafruit VL53L0X XShut set Low to Force HW Reset");
    digitalWrite(VL53LOX_ShutdownPin, LOW);
    delay(100);
    digitalWrite(VL53LOX_ShutdownPin, HIGH);
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

  // Set Interrupt Treashholds
  // Low reading set to 50mm  High Set to 100mm
  FixPoint1616_t LowThreashHold = (50 * 65536.0);
  FixPoint1616_t HighThreashHold = (100 * 65536.0);
  Serial.println("Set Interrupt Threasholds... ");
  lox.setInterruptThresholds(LowThreashHold, HighThreashHold, true);

  // Enable Continous Measurement Mode
  Serial.println("Set Mode VL53L0X_DEVICEMODE_CONTINUOUS_RANGING... ");
  lox.setDeviceMode(VL53L0X_DEVICEMODE_CONTINUOUS_RANGING, false);

  Serial.println("StartMeasurement... ");
  lox.startMeasurement();
}

void VL53LOXISR() {
  // Read if we are high or low
  VL53LOX_State = digitalRead(VL53LOX_InterruptPin);
  // set the built in LED to reflect in range on for Out of range off for in
  // range
  digitalWrite(LED_BUILTIN, VL53LOX_State);
}

void loop() {
  if (VL53LOX_State == LOW) {
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
