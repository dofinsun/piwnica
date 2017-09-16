#include "Keyboard.h"

void setup() {
  Keyboard.begin();
}

void loop() {
  delay(3000);
  Keyboard.print("user");
  Keyboard.write(KEY_RETURN);
  while (1) {
    ;  
  }
}

