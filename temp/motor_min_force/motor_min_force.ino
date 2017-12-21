const byte motor = 9;
const byte dir = 8;
const byte Button = 4;
const byte Button2 = 5;
const byte Led = 12;
int brightness = 0;
int fadeAmount = 10;
#define WaitDel 200
#define WaitDel2 30
#define WaitDel3 30

void setup() {
  pinMode(motor, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(Led, OUTPUT);
  pinMode(Button, INPUT_PULLUP);
  pinMode(Button2, INPUT_PULLUP);
  digitalWrite(motor, LOW);
  digitalWrite(dir, HIGH);
  digitalWrite(Led, LOW);
}

void loop() {
  while(!digitalRead(Button)) {
    delay(WaitDel);
  }
  while(digitalRead(Button)) {
    delay(WaitDel);
  }
  brightness += fadeAmount;
  analogWrite(motor, brightness);
}
