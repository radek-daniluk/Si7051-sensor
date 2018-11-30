/* Silicon Labs Si7051 I2C temperature sensor ±0.1°C

Simple reading and conversion to Celsius degrees functionality

According to Si datasheet one 14-bit (default resolution)
temperature conversion time is typically 7ms long and 10.8ms max.
*/
#ifndef SI7051_SENSOR_H
#define SI7051_SENSOR_H

#include <inttypes.h>
#include <debug.h>
#include <Wire.h>

class Si7051_sensor {
public:
  /* begin Wire */
  void begin(){Wire.begin();}
  /* begin Wire and set clock */
  void begin(uint32_t);
  /* Performs a measurement in hold master mode and returns temperature
  in Celsius degrees. This is the simplest way to obrain temperature by one
  function call.
  Alternatively you can call measure() followed by getResultCelsius(); */
  float readCelsius();
  /* The same as float readCelsius() but can handle errors */
  uint8_t readCelsius(float* temperature);
  /* Returns temperate obtained previously by measure() */
  float getResultCelsius();
  /* Send soft reset command */
  uint8_t reset();
protected:
  /* 16-bit temperature code returned by Si7051 */
  uint16_t temp_code;
  /* Performs a measurment and stores 16-bit result in temp_code */
  uint8_t measure();
  /* Constant addition for Celsius conversion formulas */
  const float TEMP_CONV_ADD = -46.85f;
private:
  static const uint8_t I2C_ADDRESS = 0X40;
  static const uint8_t I2C_CMD_READ_TEMP_HMM = 0XE3; // hold master mode (see Si7050 datasheet)
  static const uint8_t I2C_CMD_READ_TEMP_NHMM = 0XF3;// no hold master mode
  static const uint8_t I2C_RESET = 0XFE;

  // Constant used to convert temperature to Celcius degrees. Formula from Si7051 datasheet.
  const float TEMP_CONV_CONSTANT =  0.0026812744140625f; // 175.72/65536;
};
#endif // SI7051_SENSOR_H
