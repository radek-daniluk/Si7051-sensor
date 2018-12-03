#include <Si7051_sensor.h>
#include <Arduino.h>

void printErrorMsg(uint8_t);

Si7051_sensor si7051;

void setup() {
  Serial.begin(115200);
  si7051.begin(400000);
}

void loop() {
  Serial.print("No err handling - Temperature in Celsius deg.: ");
  Serial.println(si7051.readCelsius());
  delay(500);

  float temperature;
  uint8_t error_number = si7051.readCelsius(&temperature);
  if (error_number) {
    printErrorMsg(error_number);
  }else {
    Serial.print("Error handling  - Temperature in Celsius deg.: ");
    Serial.println(temperature);
  }
  delay(500);

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
  Serial.print("Error ");Serial.print(error_number);Serial.print(" occured: ");
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
