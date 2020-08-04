/*!
 * @file Adafruit_VL53L0X.cpp
 *
 * @mainpage Adafruit VL53L0X time-of-flight sensor
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's VL53L0X driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit VL53L0X breakout: https://www.adafruit.com/product/3317
 *
 * These sensors use I2C to communicate, 2 pins (SCL+SDA) are required
 * to interface with the breakout.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section dependencies Dependencies
 *
 *
 * @section author Author
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#include "Adafruit_VL53L0X.h"
#include "vl53l0x_api_core.h"

#define VERSION_REQUIRED_MAJOR 1 ///< Required sensor major version
#define VERSION_REQUIRED_MINOR 0 ///< Required sensor minor version
#define VERSION_REQUIRED_BUILD 1 ///< Required sensor build

#define STR_HELPER(x) #x     ///< a string helper
#define STR(x) STR_HELPER(x) ///< string helper wrapper

/**************************************************************************/
/*!
    @brief  Setups the I2C interface and hardware
    @param  i2c_addr Optional I2C address the sensor can be found on. Default is
   0x29
    @param debug Optional debug flag. If true, debug information will print out
   via Serial.print during setup. Defaults to false.
    @param  i2c Optional I2C bus the sensor is located on. Default is Wire
    @returns True if device is set up, false on any failure
*/
/**************************************************************************/
boolean Adafruit_VL53L0X::begin(uint8_t i2c_addr, boolean debug, TwoWire *i2c) {
  uint32_t refSpadCount;
  uint8_t isApertureSpads;
  uint8_t VhvSettings;
  uint8_t PhaseCal;

  // Initialize Comms
  pMyDevice->I2cDevAddr = VL53L0X_I2C_ADDR; // default
  pMyDevice->comms_type = 1;
  pMyDevice->comms_speed_khz = 400;
  pMyDevice->i2c = i2c;

  pMyDevice->i2c->begin(); // VL53L0X_i2c_init();

  // unclear if this is even needed:
  if (VL53L0X_IMPLEMENTATION_VER_MAJOR != VERSION_REQUIRED_MAJOR ||
      VL53L0X_IMPLEMENTATION_VER_MINOR != VERSION_REQUIRED_MINOR ||
      VL53L0X_IMPLEMENTATION_VER_SUB != VERSION_REQUIRED_BUILD) {
    if (debug) {
      Serial.println(F(
          "Found " STR(VL53L0X_IMPLEMENTATION_VER_MAJOR) "." STR(VL53L0X_IMPLEMENTATION_VER_MINOR) "." STR(
              VL53L0X_IMPLEMENTATION_VER_SUB) " rev " STR(VL53L0X_IMPLEMENTATION_VER_REVISION)));
      Serial.println(F("Requires " STR(VERSION_REQUIRED_MAJOR) "." STR(
          VERSION_REQUIRED_MINOR) "." STR(VERSION_REQUIRED_BUILD)));
    }

    Status = VL53L0X_ERROR_NOT_SUPPORTED;

    return false;
  }

  Status = VL53L0X_DataInit(&MyDevice); // Data initialization

  if (!setAddress(i2c_addr)) {
    return false;
  }

  Status = VL53L0X_GetDeviceInfo(&MyDevice, &DeviceInfo);

  if (Status == VL53L0X_ERROR_NONE) {
    if (debug) {
      Serial.println(F("VL53L0X Info:"));
      Serial.print(F("Device Name: "));
      Serial.print(DeviceInfo.Name);
      Serial.print(F(", Type: "));
      Serial.print(DeviceInfo.Type);
      Serial.print(F(", ID: "));
      Serial.println(DeviceInfo.ProductId);

      Serial.print(F("Rev Major: "));
      Serial.print(DeviceInfo.ProductRevisionMajor);
      Serial.print(F(", Minor: "));
      Serial.println(DeviceInfo.ProductRevisionMinor);
    }

    if ((DeviceInfo.ProductRevisionMajor != 1) ||
        (DeviceInfo.ProductRevisionMinor != 1)) {
      if (debug) {
        Serial.print(F("Error expected cut 1.1 but found "));
        Serial.print(DeviceInfo.ProductRevisionMajor);
        Serial.print(',');
        Serial.println(DeviceInfo.ProductRevisionMinor);
      }

      Status = VL53L0X_ERROR_NOT_SUPPORTED;
    }
  }

  if (Status == VL53L0X_ERROR_NONE) {
    if (debug) {
      Serial.println(F("VL53L0X: StaticInit"));
    }

    Status = VL53L0X_StaticInit(pMyDevice); // Device Initialization
  }

  if (Status == VL53L0X_ERROR_NONE) {
    if (debug) {
      Serial.println(F("VL53L0X: PerformRefSpadManagement"));
    }

    Status = VL53L0X_PerformRefSpadManagement(
        pMyDevice, &refSpadCount, &isApertureSpads); // Device Initialization

    if (debug) {
      Serial.print(F("refSpadCount = "));
      Serial.print(refSpadCount);
      Serial.print(F(", isApertureSpads = "));
      Serial.println(isApertureSpads);
    }
  }

  if (Status == VL53L0X_ERROR_NONE) {
    if (debug) {
      Serial.println(F("VL53L0X: PerformRefCalibration"));
    }

    Status = VL53L0X_PerformRefCalibration(pMyDevice, &VhvSettings,
                                           &PhaseCal); // Device Initialization
  }

  if (Status == VL53L0X_ERROR_NONE) {
    // no need to do this when we use VL53L0X_PerformSingleRangingMeasurement
    if (debug) {
      Serial.println(F("VL53L0X: SetDeviceMode"));
    }

    Status = VL53L0X_SetDeviceMode(
        pMyDevice,
        VL53L0X_DEVICEMODE_SINGLE_RANGING); // Setup in single ranging mode
  }

  // Enable/Disable Sigma and Signal check
  if (Status == VL53L0X_ERROR_NONE) {
    Status = VL53L0X_SetLimitCheckEnable(
        pMyDevice, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
  }

  if (Status == VL53L0X_ERROR_NONE) {
    Status = VL53L0X_SetLimitCheckEnable(
        pMyDevice, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
  }

  if (Status == VL53L0X_ERROR_NONE) {
    Status = VL53L0X_SetLimitCheckEnable(
        pMyDevice, VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD, 1);
  }

  if (Status == VL53L0X_ERROR_NONE) {
    Status = VL53L0X_SetLimitCheckValue(
        pMyDevice, VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD,
        (FixPoint1616_t)(1.5 * 0.023 * 65536));
  }

  if (Status == VL53L0X_ERROR_NONE) {
    return true;
  } else {
    if (debug) {
      Serial.print(F("VL53L0X Error: "));
      Serial.println(Status);
    }

    return false;
  }
}

/**************************************************************************/
/*!
    @brief  Change the I2C address of the sensor
    @param  newAddr the new address to set the sensor to
    @returns True if address was set successfully, False otherwise
*/
/**************************************************************************/
boolean Adafruit_VL53L0X::setAddress(uint8_t newAddr) {
  newAddr &= 0x7F;

  Status = VL53L0X_SetDeviceAddress(pMyDevice, newAddr * 2); // 7->8 bit

  delay(10);

  if (Status == VL53L0X_ERROR_NONE) {
    pMyDevice->I2cDevAddr = newAddr; // 7 bit addr
    return true;
  }
  return false;
}

/**************************************************************************/
/*!
    @brief  get a ranging measurement from the device
    @param  RangingMeasurementData the pointer to the struct the data will be
   stored in
    @param debug Optional debug flag. If true debug information will print via
   Serial.print during execution. Defaults to false.
    @returns True if address was set successfully, False otherwise
*/
/**************************************************************************/
VL53L0X_Error Adafruit_VL53L0X::getSingleRangingMeasurement(
    VL53L0X_RangingMeasurementData_t *RangingMeasurementData, boolean debug) {
  VL53L0X_Error Status = VL53L0X_ERROR_NONE;
  FixPoint1616_t LimitCheckCurrent;

  /*
   *  Step  4 : Test ranging mode
   */

  if (Status == VL53L0X_ERROR_NONE) {
    if (debug) {
      Serial.println(F("sVL53L0X: PerformSingleRangingMeasurement"));
    }
    Status = VL53L0X_PerformSingleRangingMeasurement(pMyDevice,
                                                     RangingMeasurementData);

    if (debug) {
      printRangeStatus(RangingMeasurementData);
    }

    if (debug) {
      VL53L0X_GetLimitCheckCurrent(pMyDevice,
                                   VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD,
                                   &LimitCheckCurrent);

      Serial.print(F("RANGE IGNORE THRESHOLD: "));
      Serial.println((float)LimitCheckCurrent / 65536.0);

      Serial.print(F("Measured distance: "));
      Serial.println(RangingMeasurementData->RangeMilliMeter);
    }
  }

  return Status;
}

/**************************************************************************/
/*!
    @brief  print a ranging measurement out via Serial.print in a human-readable
   format
    @param pRangingMeasurementData a pointer to the ranging measurement data
*/
/**************************************************************************/
void Adafruit_VL53L0X::printRangeStatus(
    VL53L0X_RangingMeasurementData_t *pRangingMeasurementData) {
  char buf[VL53L0X_MAX_STRING_LENGTH];
  uint8_t RangeStatus;

  /*
   * New Range Status: data is valid when pRangingMeasurementData->RangeStatus =
   * 0
   */

  RangeStatus = pRangingMeasurementData->RangeStatus;

  VL53L0X_GetRangeStatusString(RangeStatus, buf);

  Serial.print(F("Range Status: "));
  Serial.print(RangeStatus);
  Serial.print(F(" : "));
  Serial.println(buf);
}

/**************************************************************************/
/*!
    @brief  Single shot ranging. Be sure to check the return of {@link
   readRangeStatus} to before using the return value!
    @return Distance in millimeters if valid
*/
/**************************************************************************/

uint16_t Adafruit_VL53L0X::readRange(void) {
  Status = VL53L0X_PerformSingleRangingMeasurement(pMyDevice, &_measure);

  if (Status == VL53L0X_ERROR_NONE)
    return _measure.RangeMilliMeter;
  // Other status return something totally out of bounds...
  return 0xffff;
}

/**************************************************************************/
/*!
    @brief  Request ranging success/error message (retreive after ranging)
    @returns One of possible VL6180X_ERROR_* values
*/
/**************************************************************************/

uint8_t Adafruit_VL53L0X::readRangeStatus(void) { return _measure.RangeStatus; }

/**************************************************************************/
/*!
    @brief  Start a range operation
    @return true if range operation successfully started.
*/
/**************************************************************************/

boolean Adafruit_VL53L0X::startRange(void) {

  /* This function will do a complete single ranging
   * Here we fix the mode! */
  // first lets set the device in SINGLE_Ranging mode
  _last_status =
      VL53L0X_SetDeviceMode(pMyDevice, VL53L0X_DEVICEMODE_SINGLE_RANGING);

  if (_last_status == VL53L0X_ERROR_NONE) {
    // Lets start up the measurement
    _last_status = VL53L0X_StartMeasurement(pMyDevice);
  }
  return (_last_status == VL53L0X_ERROR_NONE);
}

/**************************************************************************/
/*!
    @brief  Checks to see if a range operation has completed
    @return true if range operation completed or an error has happened
*/
/**************************************************************************/

boolean Adafruit_VL53L0X::isRangeComplete(void) {
  uint8_t NewDataReady = 0;
  _last_status = VL53L0X_GetMeasurementDataReady(pMyDevice, &NewDataReady);
  return ((_last_status != VL53L0X_ERROR_NONE) || (NewDataReady == 1));
}

/**************************************************************************/
/*!
    @brief  Wait until Range operation has completed.
    @return true if range operation completed, false if error.
*/
/**************************************************************************/

boolean Adafruit_VL53L0X::waitRangeComplete(void) {
  _last_status = VL53L0X_measurement_poll_for_completion(pMyDevice);

  return (_last_status == VL53L0X_ERROR_NONE);
}

/**************************************************************************/
/*!
    @brief  Return the range in mm for the last operation.
    @return Range in mm.
*/
/**************************************************************************/

uint16_t Adafruit_VL53L0X::readRangeResult(void) {
  _last_status = VL53L0X_GetRangingMeasurementData(pMyDevice, &_measure);

  if (_last_status == VL53L0X_ERROR_NONE)
    _last_status = VL53L0X_ClearInterruptMask(pMyDevice, 0);

  if ((_last_status == VL53L0X_ERROR_NONE) && (_measure.RangeStatus != 4))
    return _measure.RangeMilliMeter;

  return 0xffff; // some out of range value
}

/**************************************************************************/
/*!
    @brief  Start a continuous range operation
*/
/**************************************************************************/
boolean Adafruit_VL53L0X::startRangeContinuous(uint16_t period_ms) {
  /* This function will do a complete single ranging
   * Here we fix the mode! */
  // first lets set the device in SINGLE_Ranging mode
  _last_status = VL53L0X_SetDeviceMode(
      pMyDevice, VL53L0X_DEVICEMODE_CONTINUOUS_TIMED_RANGING);

  if (_last_status == VL53L0X_ERROR_NONE) {
    _last_status =
        VL53L0X_SetInterMeasurementPeriodMilliSeconds(pMyDevice, period_ms);
  }

  if (_last_status == VL53L0X_ERROR_NONE) {
    // Lets start up the measurement
    _last_status = VL53L0X_StartMeasurement(pMyDevice);
  }
  return (_last_status == VL53L0X_ERROR_NONE);
}

/**************************************************************************/
/*!
    @brief  Stop a continuous ranging operation
*/
/**************************************************************************/
void Adafruit_VL53L0X::stopRangeContinuous(void) {

  _last_status = VL53L0X_StopMeasurement(pMyDevice);

  // lets wait until that completes.
  uint32_t StopCompleted = 0;
  uint32_t LoopNb;

  // Wait until it finished
  // use timeout to avoid deadlock
  if (_last_status == VL53L0X_ERROR_NONE) {
    LoopNb = 0;
    do {
      _last_status = VL53L0X_GetStopCompletedStatus(pMyDevice, &StopCompleted);
      if ((StopCompleted == 0x00) || _last_status != VL53L0X_ERROR_NONE) {
        break;
      }
      LoopNb = LoopNb + 1;
      VL53L0X_PollingDelay(pMyDevice);
    } while (LoopNb < VL53L0X_DEFAULT_MAX_LOOP);

    if (LoopNb >= VL53L0X_DEFAULT_MAX_LOOP) {
      _last_status = VL53L0X_ERROR_TIME_OUT;
    }
  }

  if (_last_status == VL53L0X_ERROR_NONE) {
    _last_status = VL53L0X_ClearInterruptMask(
        pMyDevice, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
  }
}
