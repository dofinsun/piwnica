const byte motor = 9;
const byte dir = 8;
const byte Button = 4;
const byte Button2 = 5;
const byte Led = 12;
int brightness = 50;
int fadeAmount = 5;
#define WaitDel 200
#define WaitDel2 30
#define WaitDel3 30

void setup() {
  pinMode(motor, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(Led, OUTPUT);
  pinMode(Button, INPUT_PULLUP);
  pinMode(Button2, INPUT_PULLUP);
}

void loop() {
  digitalWrite(motor, LOW);
  digitalWrite(dir, HIGH);
  digitalWrite(Led, LOW);
  while(!digitalRead(Button)) {
    delay(WaitDel);
  }
  while(digitalRead(Button)) {
    delay(WaitDel);
  }
  digitalWrite(Led, HIGH);
  brightness = 120;
  for (int i = 0;  i < 20; i++) {
    brightness += fadeAmount;
    analogWrite(motor, brightness);
    delay(WaitDel2);
  }
  delay(100);
  digitalWrite(Led, LOW);
  for (int i = 0;  i < 40; i++) {
    brightness -= fadeAmount;
    analogWrite(motor, brightness);
    delay(WaitDel3);
  }
  digitalWrite(Led, HIGH);
  while(digitalRead(Button)) {
    delay(20);
  }
  digitalWrite(motor, LOW);
  digitalWrite(dir, LOW);
  digitalWrite(Led, LOW);

  while(!digitalRead(Button2)) {
    delay(WaitDel);
  }
  while(digitalRead(Button2)) {
    delay(WaitDel);
  }
  digitalWrite(Led, HIGH);
  brightness = 120;
  for (int i = 0;  i < 20; i++) {
    brightness += fadeAmount;
    analogWrite(motor, brightness);
    delay(WaitDel2);
  }
  delay(100);
  digitalWrite(Led, LOW);
  for (int i = 0;  i < 40; i++) {
    brightness -= fadeAmount;
    analogWrite(motor, brightness);
    delay(WaitDel3);
  }
  digitalWrite(Led, HIGH);
  while(digitalRead(Button2)) {
    delay(20);
  }
  digitalWrite(motor, LOW);
  digitalWrite(Led, LOW);
}
