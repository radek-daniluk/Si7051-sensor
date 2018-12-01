#include <Si7051_sensor.h>
#include <Arduino.h>

Si7051_sensor si7051;

void setup() {
  Serial.begin(115200);
  si7051.begin(400000);
}

void loop() {
  Serial.print("No error handling - Temperature in Celsius deg.: ");
  Serial.println(si7051.readCelsius());
  delay(200);

  float temperature;
  uint8_t error_number = si7051.readCelsius(&temperature);
  if (error_number) {
    Serial.print("Error ");Serial.print(error_number);Serial.print(" occured: ");
    switch(error_number) {
      case 1: Serial.println("data too long to fit in transmit buffer"); break;
      case 2: Serial.println("received NACK on transmit of address"); break;
      case 3: Serial.println("received NACK on transmit of data"); break;
      case 4: Serial.println("other error from Wire.endTransmission()"); break;
      case 5: Serial.println("incorrect CRC"); break;
      default: Serial.println("Unknown");
    }
  }else {
    Serial.print("Error handling - Temperature in Celsius deg.: ");
    Serial.println(temperature);
  }

  delay(1000);
}
