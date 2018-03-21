/*!
 * @file Adafruit_VL53L0X.h

  This is a library for the Adafruit VL53L0X Sensor Breakout

  Designed specifically to work with the VL53L0X sensor from Adafruit
  ----> https://www.adafruit.com/products/3317

  These sensors use I2C to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  Updated by Andrew DeVries for Digital Example to include methods needed for Interrupt triggering
 ****************************************************/

#ifndef ADAFRUIT_VL53L0X_H
#define ADAFRUIT_VL53L0X_H

#if ( ARDUINO >= 100 )
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "Wire.h"
#include "vl53l0x_api.h"

#define VL53L0X_I2C_ADDR  0x29 ///< Default sensor I2C address

/**************************************************************************/
/*! 
    @brief  Class that stores state and functions for interacting with VL53L0X time-of-flight sensor chips
*/
/**************************************************************************/
class Adafruit_VL53L0X
{
  public:
    boolean       begin(uint8_t i2c_addr = VL53L0X_I2C_ADDR, boolean debug = false );
    boolean       setAddress(uint8_t newAddr);

    /**************************************************************************/
    /*! 
        @brief  get a ranging measurement from the device
        @param  pRangingMeasurementData the pointer to the struct the data will be stored in
        @param debug Optional debug flag. If true debug information will print via Serial.print during execution. Defaults to false.
        @returns True if address was set successfully, False otherwise
    */
    /**************************************************************************/
    VL53L0X_Error 
      rangingTest(VL53L0X_RangingMeasurementData_t* pRangingMeasurementData, 
		  boolean debug = false) 
    { getSingleRangingMeasurement(pRangingMeasurementData, debug); };

    VL53L0X_Error getSingleRangingMeasurement( VL53L0X_RangingMeasurementData_t* pRangingMeasurementData, boolean debug = false );
    void          printRangeStatus( VL53L0X_RangingMeasurementData_t* pRangingMeasurementData );

    VL53L0X_Error getRangingMeasurement( VL53L0X_RangingMeasurementData_t* pRangingMeasurementData, boolean debug = false );
    VL53L0X_Error startMeasurement( boolean debug = false );
    VL53L0X_Error stopMeasurement( boolean debug = false );
    VL53L0X_Error setLimitCheckEnable( uint8_t LimitCheckId, boolean LimitCheckEnable, boolean debug = false );
    boolean getLimitCheckEnable( uint8_t LimitCheckId, boolean debug = false );

    VL53L0X_Error getLimitCheckCurrent( uint8_t LimitCheckId, FixPoint1616_t *pLimitCheckCurrent, boolean debug = false );
    VL53L0X_Error getLimitCheckValue( uint8_t LimitCheckId, FixPoint1616_t *pLimitCheckValue, boolean debug = false );
    VL53L0X_Error setLimitCheckValue( uint8_t LimitCheckId, FixPoint1616_t LimitCheckValue, boolean debug = false );			  
    VL53L0X_Error getDeviceMode( VL53L0X_DeviceModes *pDeviceMode, boolean debug = false );
    VL53L0X_Error setDeviceMode( VL53L0X_DeviceModes DeviceMode, boolean debug = false );

    VL53L0X_Error setInterruptThresholds( FixPoint1616_t ThresholdLow, FixPoint1616_t ThresholdHigh, boolean debug = false );
    VL53L0X_Error getInterruptThresholds( FixPoint1616_t *pThresholdLow, FixPoint1616_t *pThresholdHigh, boolean debug = false );
    VL53L0X_Error clearInterruptMask(boolean debug = false );

    VL53L0X_Error getGpioConfig(VL53L0X_DeviceModes *pDeviceMode, VL53L0X_GpioFunctionality *pFunctionality, VL53L0X_InterruptPolarity *pPolarity, boolean debug = false );
    VL53L0X_Error setGpioConfig(VL53L0X_DeviceModes DeviceMode, VL53L0X_GpioFunctionality Functionality, VL53L0X_InterruptPolarity Polarity, boolean debug = false );
			

    VL53L0X_Error                     Status      = VL53L0X_ERROR_NONE; ///< indicates whether or not the sensor has encountered an error

   

 private:
  VL53L0X_Dev_t                       MyDevice;
  VL53L0X_Dev_t                       *pMyDevice  = &MyDevice;
  VL53L0X_Version_t                   Version;
  VL53L0X_Version_t                   *pVersion   = &Version;
  VL53L0X_DeviceInfo_t                DeviceInfo;
};

#endif