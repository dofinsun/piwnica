#include <TM1637Display.h>
#define CLK A4
#define DIO A5
#define Button1 10
#define Led1 9
#define Key1 8
#define ShowDel 100
#define WaitDel 200

const uint8_t hasp[] = {
  B01110110, //H
  B01011111, //a
  B01101101, //S
  B01110011, //P
};
const uint8_t openz[] = {
  B00111111, //O
  B01110011, //P
  B01111001, //E
  B00110111, //N
};

TM1637Display display(CLK, DIO);

void setup() {
  pinMode(Key1, OUTPUT);
  digitalWrite(Key1, LOW);
  pinMode(Button1, INPUT_PULLUP);
  pinMode(Led1, OUTPUT);
  digitalWrite(Led1, LOW);
  display.setBrightness(7, true);
  randomSeed(analogRead(A0));
}

void loop() {
  underun();
  display.setSegments(hasp);
  while(!digitalRead(Button1)) {
    delay(WaitDel);
  }
  while(digitalRead(Button1)) {
    delay(WaitDel);
  }
  brutforce();
  display.showNumberDec(7142, false);
  digitalWrite(Led1, HIGH);
  while(!digitalRead(Button1)) {
    delay(WaitDel);
  }
  while(digitalRead(Button1)) {
    delay(WaitDel);
  }
  display.setSegments(openz);
  digitalWrite(Key1, HIGH);
  digitalWrite(Led1, LOW);
  while(1);
}

void underun() {
  uint8_t data[] = { B00001000, 0, 0, 0 };
  for (char i=0; i <= 3; i++){
     for (char k=0; k <= 2; k++){
        byte l = k + 1;
        display.setSegments(data);
        data[l]=data[k];
        data[k]=0;
        delay(ShowDel);
     }
     display.setSegments(data);
     delay(ShowDel);
     for (char k=2; k >= 0; k--){
        byte l = k + 1;
        display.setSegments(data);
        data[k]=data[l];
        data[l]=0;
        delay(ShowDel);
      }
      display.setSegments(data);
      delay(ShowDel);
   } 
}

void brutforce() {
  uint8_t data[] = { 0, 0, 0, 0 };
  for (char i=0; i <= 100; i++){
    for (char x=0; x <= 3; x++){
      data[x]= display.encodeDigit(random(10));
    }
    display.setSegments(data);
    delay(ShowDel);
  }
}

