#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
}

void loop() {
  for (byte i = 0; i < 3; i++) {
    Wire.beginTransmission(16);
    Wire.write(i);
    Wire.endTransmission();
    delay(1);
    Serial.print(i);
    Serial.print(" ");
    Wire.requestFrom(16, 4);
    while (Wire.available()) {
      byte c = Wire.read();
      Serial.print(String(c,HEX));
    }
    Serial.println();
    delay(1);
  }
  delay(999);
}
