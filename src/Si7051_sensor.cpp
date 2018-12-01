#include <Si7051_sensor.h>
#include <Arduino.h>

//TODO replace delay with AVR sleep states
// add equal delays between temp. readings in measurLoop

void Si7051_sensor::begin(uint32_t clock) {
  begin();
  Wire.setClock(clock);
}

float Si7051_sensor::readCelsius() {
  measure();
  return TEMP_CONV_CONSTANT*temp_code+TEMP_CONV_ADD;
}

uint8_t Si7051_sensor::readCelsius(float* temperature) {
  uint8_t err = measure(true);
  if(err) {
    DEBUG("ERROR in Si7051_sensor::readCelsius: Read temperature I2C error=");DEBUGLN(err);
    return err;
  }
  *temperature = TEMP_CONV_CONSTANT*temp_code+TEMP_CONV_ADD;
  return err;
}

uint8_t Si7051_sensor::beginMeasure() {
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write(I2C_CMD_READ_TEMP_NHMM);
    return Wire.endTransmission();
}

uint8_t Si7051_sensor::getResultCelsius(float* temperature, bool check_crc) {
  uint8_t data_length;
  if(check_crc){
    data_length = 3;
  }else {
    data_length = 2;
  }
  if( Wire.requestFrom(I2C_ADDRESS, data_length) != data_length ) {
    return SI7051_NOT_READY_ERR;
  }else {
    temp_code = Wire.read()<<8|Wire.read(); // read 16-bit temperature code
    if ( check_crc && checkTempCRC( Wire.read() ) ) {
      return SI7051_INCORRECT_CRC_ERR;
    }
    *temperature = TEMP_CONV_CONSTANT*temp_code+TEMP_CONV_ADD;
  }
  return 0;
}

uint8_t Si7051_sensor::measure(bool check_crc) {
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(I2C_CMD_READ_TEMP_HMM);
  uint8_t err = Wire.endTransmission(false);
  if(err) {
    return err;
  }else {
    uint8_t data_length;
    if(check_crc){
      data_length = 3;
    }else {
      data_length = 2;
    }
    Wire.requestFrom(I2C_ADDRESS, data_length);
    Wire.available();
    temp_code = Wire.read()<<8|Wire.read(); // read 16-bit temperature code

    if ( check_crc && checkTempCRC( Wire.read() ) ) {
      return SI7051_INCORRECT_CRC_ERR;
    }
  }
  return err;
}

uint8_t Si7051_sensor::reset() {
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(I2C_RESET);
  return Wire.endTransmission();
}

/* ************************************
private methods
*/

uint8_t Si7051_sensor::checkTempCRC(const uint8_t crc) {
  // Serial.print(temp_code, HEX); Serial.print(" CRC="); Serial.println(crc, HEX);

  uint16_t temp_code_crc = temp_code;
  uint8_t* const msb = (uint8_t*)&temp_code_crc + 1; // set pointer to MS byte
  for (uint8_t i = 0; i < 16; i++) {
    if (( *msb & 0x80 ) != 0 ) {
      temp_code_crc <<= 1;
      *msb ^= 0x31;
    }else{
      temp_code_crc <<= 1;
    }
  }
  // Serial.print(temp_code_crc, HEX); Serial.print("<-temp_code_crc *msb->"); Serial.println(*msb, HEX);
  return crc != *msb;
}
