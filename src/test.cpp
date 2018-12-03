#include <Si7051_sensor.h>
#include <Arduino.h>

void printErrorMsg(uint8_t);

Si7051_sensor si7051;

void setup() {
  Serial.begin(115200);
  si7051.begin(400000);
}

void loop() {
  //firmware revision read
  float firmware_rev;
  uint8_t error_number = si7051.readFirmwareRev(&firmware_rev);
  if(error_number) {
    printErrorMsg(error_number);
  }else {
    Serial.print("Si7051 Firmaware revision: ");
    if(firmware_rev == -1.0f) {
      Serial.println("unknown");
    }else {
      Serial.println(firmware_rev, 1);
    }
  }

  // Simplest temperature measurement without error handling
  Serial.print("No err handling - Temperature in Celsius deg.: ");
  Serial.println(si7051.readCelsius());
  delay(500);

  // Simple temperature measurement with error handgling
  float temperature;
  error_number = si7051.readCelsius(&temperature);
  if (error_number) {
    printErrorMsg(error_number);
  }else {
    Serial.print("Error handling  - Temperature in Celsius deg.: ");
    Serial.println(temperature);
  }
  delay(500);

  // Two step temperature measurement.
  // You have several (max 11) miliseconds before temperate is ready to read.
  error_number = si7051.beginMeasure();
  if (error_number) {
    printErrorMsg(error_number);
  }else{
    delay(11); // wait for Si7051 temp. conversion to complete
    if( (error_number = si7051.getResultCelsius(&temperature, true) ) ) {
      printErrorMsg(error_number);
    }else {
      Serial.print("begin-getResult - Temperature in Celsius deg.: ");
      Serial.println(temperature);
    }
  }

  delay(1000);
}

void printErrorMsg(uint8_t error_number) {
  Serial.print("Error ");Serial.print(error_number);Serial.print(": ");
  switch(error_number) {
    case 1: Serial.println("data too long to fit in transmit buffer"); break;
    case 2: Serial.println("received NACK on transmit of address"); break;
    case 3: Serial.println("received NACK on transmit of data"); break;
    case 4: Serial.println("other error from Wire.endTransmission()"); break;
    case SI7051_INCORRECT_CRC_ERR: Serial.println("incorrect CRC"); break;
    case SI7051_NOT_READY_ERR: Serial.println("Result not ready"); break;
    default: Serial.println("Unknown");
  }
}
