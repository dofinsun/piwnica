const byte LED1 = 9;
const byte LED2 = 10;
const byte LED3 = 11;
const byte LED4 = 12;
const byte LED5 = 13;
const byte LEDred = 5;
const byte LEDgreen = 6;
const byte Button = 4;
#define WaitDel 200

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LEDred, OUTPUT);
  pinMode(LEDgreen, OUTPUT);
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
  digitalWrite(LED5, HIGH);
  digitalWrite(LEDgreen, HIGH);
  digitalWrite(LEDred, LOW);
  pinMode(Button, INPUT_PULLUP);
}

void loop() {
  while(!digitalRead(Button)) {
    delay(WaitDel);
  }
  while(digitalRead(Button)) {
    delay(WaitDel);
  }
  int i;
  int myDelay[] = {50, 200, 50, 250, 100, 50, 20};
  for(i = 0; i <=5; i++) {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED5, LOW);
    digitalWrite(LEDgreen, LOW);
    delay(myDelay[i]);
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, HIGH);
    digitalWrite(LED5, HIGH);
    digitalWrite(LEDgreen, HIGH);
    delay(myDelay[i]);
  }
  for(i = 255; i >= 0; i=i-5) {
    analogWrite(LED1, i);
    analogWrite(LED2, i);
    analogWrite(LED3, i);
    analogWrite(LED4, i);
    analogWrite(LED5, i);
    analogWrite(LEDgreen, i);
    delay(10);
    }
  delay(1000);
  digitalWrite(LED1, HIGH);
  delay(500);
  digitalWrite(LED2, HIGH);
  delay(500);
  digitalWrite(LED3, HIGH);
  delay(500);
  digitalWrite(LED4, HIGH);
  digitalWrite(LEDgreen, HIGH);
  delay(500);
  digitalWrite(LED5, HIGH);
  delay(500);
  for(i = 0; i <=5; i++) {
    digitalWrite(LEDgreen, LOW);
    delay(myDelay[i]);
    digitalWrite(LEDgreen, HIGH);
    delay(myDelay[i]);
  }
  digitalWrite(LEDgreen, LOW);
  delay(500);
  digitalWrite(LEDred, HIGH);
  delay(2000);
   for(i = 255; i >= 0; i=i-5) {
    analogWrite(LEDred, i);
    delay(10);
   }
  digitalWrite(LEDred, LOW);
  delay(2000);
  int myPass[] = {3, 5, 2, 7};
  int morzeDelay1 = 400;
  int morzeDelay2 = morzeDelay1 * 3;
  int morzeDelay3 = 6000;
    for(char x=0; x<=3; x++) {
      for(char y=myPass[x]; y>0; y--) {
        digitalWrite(LEDred, HIGH);
        delay(morzeDelay1);
        digitalWrite(LEDred, LOW);
        delay(morzeDelay1);
      }
      delay(morzeDelay2);
    }
    delay(morzeDelay3);
    digitalWrite(LEDgreen, HIGH);
}
