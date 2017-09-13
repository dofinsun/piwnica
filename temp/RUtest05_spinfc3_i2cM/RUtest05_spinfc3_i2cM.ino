#include <Wire.h>

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  while (!Serial) {
    ;
  }
}

void loop() {
  for (int i = 0; i < 3; i++) {
    Wire.beginTransmission(16);
    Wire.write(i);
    Wire.endTransmission();
    delay(10);
    Serial.print(i);
    Serial.print(" ");
    Wire.requestFrom(16, 4);
    while (Wire.available()) {
      byte c = Wire.read();
      Serial.print(String(c,HEX));
    }
    Serial.println();
    delay(10);
  }
  delay(900);
}
