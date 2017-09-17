/* ----- Pin setting -----
   PC4 - Analog   4 - Encoder SDA
   PC5 - Analog   5 - Encoder SCL
*/

#include <stdint.h>
#include <Wire.h>

#define AS5600_AS5601_DEV_ADDRESS      0x36
#define AS5600_AS5601_REG_RAW_ANGLE    0x0C

void setup() {
  // I2C init
  Wire.begin();
  Wire.setClock(400000);

  // Communication function init
  Serial.begin(115200);
  while (!Serial);
}

void loop() {
  // Read RAW_ANGLE value from encoder
  Wire.beginTransmission(AS5600_AS5601_DEV_ADDRESS);
  Wire.write(AS5600_AS5601_REG_RAW_ANGLE);
  Wire.endTransmission(false);
  Wire.requestFrom(AS5600_AS5601_DEV_ADDRESS, 2);
  uint16_t RawAngle = 0;
  RawAngle  = ((uint16_t)Wire.read() << 8) & 0x0F00;
  RawAngle |= (uint16_t)Wire.read();
  // Raw angle value (0x0000~0x0FFF) is stored in RawAngle

  Serial.print("Raw angle value = ");
  Serial.println(RawAngle);
  delay(200);
}

