/* Silicon Labs Si7051 I2C temperature sensor ±0.1°C

Simple reading and conversion to Celsius degrees functionality

*/
#ifndef SI7051_SENSOR_H
#define SI7051_SENSOR_H

#define SI7051_INCORRECT_CRC_ERR 5
#define SI7051_NOT_READY_ERR 6

#include <inttypes.h>
#include <debug.h>
#include <Wire.h>

class Si7051_sensor {
public:
  /*
  Begin Wire */
  void begin(){Wire.begin();}
  /*
  Begin Wire and set clock */
  void begin(uint32_t);
  /*
  Perform a measurement in hold master mode and return temperature
  in Celsius degrees. This is the simplest way to obtain temperature. */
  float readCelsius();
  /*
  The same as float readCelsius() but can handle errors */
  uint8_t readCelsius(float* const temperature);
  /*
  Begin a measurment in NO hold master mode. To get result use getResultCelsius()
  According to SI7051 datasheet one 14-bit (default resolution) temperature
  conversion time is typically 7ms long and 10.8ms max. Using beginMeasure()/
  getResultCelsius() pair, user must handle this time. */
  uint8_t beginMeasure();
  /*
  Return temperature in Celsius. Measurement must be initiated by beginMeasure().
  If result is not ready function getResultCelsius() will return SI7051_NOT_READY_ERR
  Note if you call getResultCelsius() without beginMeasure() the function will
  return last correctly obtained temperature value */
  uint8_t getResultCelsius(float* const temperature, bool check_crc=0);
  /*
  Send soft reset command to Si7051 */
  uint8_t reset();
  /*
  Read firmware revision code returned by Si7051 */
  uint8_t readFirmwareRevCode(uint8_t* const);
  /*
  Read firmaware revision as float number e.g. 2.0 */
  uint8_t readFirmwareRev(float* const);
  /*
  Read 64-bits (8-bytes) long Si7051 Electrnic ID (serial number) */
  uint8_t readEID(uint8_t* const, bool check_crc=1);
protected:
  /*
  Perform a measurment in hold master mode and store 16-bit result in temp_code */
  uint8_t measure(bool check_crc=0);
  /* 16-bit temperature code returned by Si7051 */
  uint16_t temp_code;
  /* Constant addition for Celsius conversion formulas */
  const float TEMP_CONV_ADD = -46.85f;
private:
  static const uint8_t I2C_ADDRESS = 0X40;
  static const uint8_t I2C_CMD_READ_TEMP_HMM = 0XE3; // hold master mode (see Si7050 datasheet)
  static const uint8_t I2C_CMD_READ_TEMP_NHMM = 0XF3;// no hold master mode
  // Constant used to convert temperature to Celcius degrees. Formula from Si7051 datasheet.
  const float TEMP_CONV_CONSTANT =  0.0026812744140625f; // 175.72/65536;
  uint8_t crc8(uint8_t, uint8_t);
  uint8_t checkTempCRC(uint8_t);
  uint8_t send2byteCmd(uint8_t, uint8_t);
  uint8_t get4bytesEID(uint8_t* const, bool, bool check_crc=1);
};
#endif // SI7051_SENSOR_H
