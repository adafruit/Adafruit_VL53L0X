#include "Adafruit_VL53L0X.h"

#define VERSION_REQUIRED_MAJOR 1
#define VERSION_REQUIRED_MINOR 0
#define VERSION_REQUIRED_BUILD 1
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


boolean Adafruit_VL53L0X::begin(void) {
  int32_t status_int;
  int32_t init_done = 0;

  // Initialize Comms
  pMyDevice->I2cDevAddr      = 0x29;  // 7 bit addr
  pMyDevice->comms_type      =  1;
  pMyDevice->comms_speed_khz =  400;

  
  Wire.begin();  // VL53L0X_i2c_init();

  // unclear if this is even needed:
  if( VL53L0X_IMPLEMENTATION_VER_MAJOR != VERSION_REQUIRED_MAJOR ||
      VL53L0X_IMPLEMENTATION_VER_MINOR != VERSION_REQUIRED_MINOR ||
      VL53L0X_IMPLEMENTATION_VER_SUB != VERSION_REQUIRED_BUILD )  {

    Serial.println(F("Found " STR(VL53L0X_IMPLEMENTATION_VER_MAJOR) "." STR(VL53L0X_IMPLEMENTATION_VER_MINOR) "."  STR(VL53L0X_IMPLEMENTATION_VER_SUB) " rev " STR(VL53L0X_IMPLEMENTATION_VER_REVISION)));
    Serial.println(F("Requires " STR(VERSION_REQUIRED_MAJOR) "." STR(VERSION_REQUIRED_MINOR) "." STR(VERSION_REQUIRED_BUILD)));
    return false;
  }

  Status = VL53L0X_DataInit(&MyDevice); // Data initialization

  Status = VL53L0X_GetDeviceInfo(&MyDevice, &DeviceInfo);
  if (Status != VL53L0X_ERROR_NONE)
    return false;

  if(Status == VL53L0X_ERROR_NONE)  {
  /*
     Serial.println(F("VL53L0X_GetDeviceInfo:"));
     Serial.print(F("Device Name : ")); Serial.println(DeviceInfo.Name);
     Serial.print(F("Device Type : ")); Serial.println(DeviceInfo.Type);
     Serial.print(F("Device ID : ")); Serial.println(DeviceInfo.ProductId);
     Serial.print(F("ProductRevisionMajor : ")); Serial.println(DeviceInfo.ProductRevisionMajor);
     Serial.print(F("ProductRevisionMinor : ")); Serial.println(DeviceInfo.ProductRevisionMinor);
  */
     if ((DeviceInfo.ProductRevisionMinor != 1) && (DeviceInfo.ProductRevisionMinor != 1)) {
       /*
       Serial.print(F("Error expected cut 1.1 but found cut ")); 
       Serial.print(DeviceInfo.ProductRevisionMajor);
       Serial.print('.');
       Serial.println(DeviceInfo.ProductRevisionMinor);
       */
       Status = VL53L0X_ERROR_NOT_SUPPORTED;
       return false;     
     }
  }
  return true;
}


VL53L0X_Error Adafruit_VL53L0X::rangingTest(void)
{
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    VL53L0X_RangingMeasurementData_t    RangingMeasurementData;
    int i;
    FixPoint1616_t LimitCheckCurrent;
    uint32_t refSpadCount;
    uint8_t isApertureSpads;
    uint8_t VhvSettings;
    uint8_t PhaseCal;

    if(Status == VL53L0X_ERROR_NONE)
    {
        Serial.println(F("Call of VL53L0X_StaticInit"));
        Status = VL53L0X_StaticInit(pMyDevice); // Device Initialization
    }
    
    if(Status == VL53L0X_ERROR_NONE)
    {
        Serial.println(F("Call of VL53L0X_PerformRefCalibration"));
        Status = VL53L0X_PerformRefCalibration(pMyDevice,
            &VhvSettings, &PhaseCal); // Device Initialization
    }

    if(Status == VL53L0X_ERROR_NONE)
    {
        Serial.println(F("Call of VL53L0X_PerformRefSpadManagement"));
        Status = VL53L0X_PerformRefSpadManagement(pMyDevice,
            &refSpadCount, &isApertureSpads); // Device Initialization
        Serial.print(F("refSpadCount = ")); 
        Serial.print(refSpadCount);
        Serial.print(F(", isApertureSpads = "));
        Serial.println(isApertureSpads);
    }

    if(Status == VL53L0X_ERROR_NONE)
    {

        // no need to do this when we use VL53L0X_PerformSingleRangingMeasurement
        Serial.println(F("Call of VL53L0X_SetDeviceMode"));
        Status = VL53L0X_SetDeviceMode(pMyDevice, VL53L0X_DEVICEMODE_SINGLE_RANGING); // Setup in single ranging mode
    }

    // Enable/Disable Sigma and Signal check
    if (Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_SetLimitCheckEnable(pMyDevice,
            VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
    }
    if (Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_SetLimitCheckEnable(pMyDevice,
            VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
    }

    if (Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_SetLimitCheckEnable(pMyDevice,
            VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD, 1);
    }

    if (Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_SetLimitCheckValue(pMyDevice,
            VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD,
            (FixPoint1616_t)(1.5*0.023*65536));
    }


    /*
     *  Step  4 : Test ranging mode
     */

    if(Status == VL53L0X_ERROR_NONE)
    {
        for(i=0;i<10;i++){
            Serial.println(F("Call of VL53L0X_PerformSingleRangingMeasurement"));
            Status = VL53L0X_PerformSingleRangingMeasurement(pMyDevice,
                &RangingMeasurementData);

            print_range_status(&RangingMeasurementData);

            VL53L0X_GetLimitCheckCurrent(pMyDevice,
                VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD, &LimitCheckCurrent);

            Serial.print(F("RANGE IGNORE THRESHOLD: "));
            Serial.println((float)LimitCheckCurrent/65536.0);

            if (Status != VL53L0X_ERROR_NONE) break;

            Serial.print(F("Measured distance: "));
            Serial.println(RangingMeasurementData.RangeMilliMeter);
        }
    }
    return Status;
}




void Adafruit_VL53L0X::print_range_status(VL53L0X_RangingMeasurementData_t* pRangingMeasurementData){
    char buf[VL53L0X_MAX_STRING_LENGTH];
    uint8_t RangeStatus;

    /*
     * New Range Status: data is valid when pRangingMeasurementData->RangeStatus = 0
     */

    RangeStatus = pRangingMeasurementData->RangeStatus;

    VL53L0X_GetRangeStatusString(RangeStatus, buf);
    Serial.print(F("Range Status: "));
    Serial.print(RangeStatus);
    Serial.print(" : "); 
    Serial.println(buf);

}

