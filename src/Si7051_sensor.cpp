#include <Si7051_sensor.h>
#include <Arduino.h>

void Si7051_sensor::begin(uint32_t clock) {
  begin();
  Wire.setClock(clock);
}

float Si7051_sensor::readCelsius() {
  measure();
  return TEMP_CONV_CONSTANT*temp_code+TEMP_CONV_ADD;
}

uint8_t Si7051_sensor::readCelsius(float* const temperature) {
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

uint8_t Si7051_sensor::getResultCelsius(float* const temperature, bool check_crc) {
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
    temp_code = Wire.read()<<8|Wire.read(); // read 16-bit temperature code

    if ( check_crc && checkTempCRC( Wire.read() ) ) {
      return SI7051_INCORRECT_CRC_ERR;
    }
  }
  return err;
}

uint8_t Si7051_sensor::reset() {
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(0xFE);
  return Wire.endTransmission();
}

uint8_t Si7051_sensor::readFirmwareRevCode(uint8_t* const firmware_rev_code) {
  Wire.beginTransmission(I2C_ADDRESS);
	Wire.write(0x84);
	Wire.write(0xB8);
  uint8_t err = Wire.endTransmission();
	if(err) {
    return err;
  }
	Wire.requestFrom(I2C_ADDRESS, (uint8_t)1);
	*firmware_rev_code = Wire.read();
  return err;
}

uint8_t Si7051_sensor::readFirmwareRev(float* const firmware_rev) {
  uint8_t firmware_rev_code, err;
  err = readFirmwareRevCode(&firmware_rev_code);
  if(err) {
    return err;
  }
  switch (firmware_rev_code) {
    case 0xFF: *firmware_rev = 1.0f; break;
    case 0x20: *firmware_rev = 2.0f; break;
    default:  *firmware_rev = -1.0f; break; // unknown
  }
  return err;
}

uint8_t Si7051_sensor::readEID(uint8_t* const EID_bytes, bool check_crc) {

  uint8_t err = send2byteCmd(0xFA, 0x0F);
  if(err) return err;
  err = get4bytesEID(EID_bytes, 0, check_crc);
  if(err) return err;
  err = send2byteCmd(0xFC, 0xC9);
  if(err) return err;
  err = get4bytesEID(EID_bytes+4, 1, check_crc);
  // if(err) return err;

  return err;
}

/* ************************************
private methods
*/

uint8_t Si7051_sensor::checkTempCRC(uint8_t crc) {
  // DEBUG("checkCRC temp_code="); DEBUG_HEX(temp_code); DEBUG(" CRC="); DEBUG_HEX(crc);

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
  // DEBUG(" | temp_code_crc=");DEBUG_HEX(temp_code_crc); DEBUG(" msb="); DEBUG_HEX(*msb); DEBUGLN();
  return crc != *msb;
}

uint8_t Si7051_sensor::crc8(uint8_t data, uint8_t crc) {
  // DEBUG("checkCRC: data="); DEBUG_HEX(data);
  data ^= crc;
  for (uint8_t i = 0; i < 8; i++) {
    if (( data & 0x80 ) != 0 ) {
      data <<= 1;
      data ^= 0x31;
    }else{
      data <<= 1;
    }
  }
  // DEBUG(" | crc calcul="); DEBUG_HEX(data); DEBUGLN();
  return crc = data;
}

uint8_t Si7051_sensor::send2byteCmd(uint8_t first_byte, uint8_t second_byte) {
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(first_byte);
  Wire.write(second_byte);
  return Wire.endTransmission();
}

uint8_t Si7051_sensor::get4bytesEID(uint8_t* const EID_bytes, bool second_cmd, bool check_crc) {
  Wire.requestFrom(I2C_ADDRESS, uint8_t(8));

  uint8_t crc=0;
  for(uint8_t i=0; i<4; i++) {
    EID_bytes[i]=Wire.read();
    crc = crc8(EID_bytes[i], crc);
    if( second_cmd && (i==0 || i==2) ) {
      // DEBUGLN("Skip crc check for 0 or 2 byte in second command");
      ;
    }else if (crc != Wire.read() ) {
        DEBUGLN("EID CRC error");
        return SI7051_INCORRECT_CRC_ERR;
    }
  }
  return 0;
}
