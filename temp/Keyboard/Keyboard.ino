#include "Keyboard.h"

void setup() {
  Keyboard.begin();
}

void loop() {
  delay(3000);
  Keyboard.print("user");
  Keyboard.write(KEY_RETURN);
  delay(3000);
  Keyboard.write(KEY_F12);
  while (1) {
    ;  
  }
}


