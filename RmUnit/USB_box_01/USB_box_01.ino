const int row[4] = { 14, 15, 16, 17 };
const int col[8] = { 2, 3, 4, 5, 6, 7, 8, 9 };
int Button1 = 18;
int Led1 = 19;
int Key1 = 10;
boolean isBitSet;
int i = 0;
const byte numeral[13]= {
B11011010, //H
B10111111, //a
B11100110, //S
B11111000, //P
B00000001, //.
B00110010, //7
B11110110, //9
B10111100, //2
B11100110, //5
B01111110, //O
B11111000, //P
B11101100, //E
B01111010, //N
};

void setup() {
  pinMode(Key1, OUTPUT);
  digitalWrite(Key1, LOW);
  for (int thisPin = 0; thisPin < 4; thisPin++) {
    pinMode(row[thisPin], OUTPUT);
    digitalWrite(row[thisPin], HIGH);
  }
  for (int thisPin = 0; thisPin < 8; thisPin++) {
    pinMode(col[thisPin], OUTPUT);
    digitalWrite(col[thisPin], LOW);
  }
  pinMode(Button1, INPUT_PULLUP);
  pinMode(Led1, OUTPUT);
  digitalWrite(Led1, LOW);
}

void loop() {
  for (int Segment = 0; Segment < 4; Segment++) {
    for (int thisPin = 0; thisPin < 8; thisPin++) {
      isBitSet = bitRead(numeral[Segment+i], thisPin);
      digitalWrite(col[thisPin], isBitSet);
    }
    digitalWrite(row[Segment], LOW);
    delay(2);
    digitalWrite(row[Segment], HIGH);
  }
  int sensorVal = digitalRead(Button1);
  if (sensorVal == LOW) {
    if (i == 5){
      digitalWrite(Led1, LOW);
      i = 9;
      digitalWrite(Key1, HIGH);
    }
    if (i == 0){
      for (int thisPin = 0; thisPin < 8; thisPin++) {
        isBitSet = bitRead(numeral[4], thisPin);
        digitalWrite(col[thisPin], isBitSet);
      }
      for (int j = 0; j < 7; j++){
        for (int Segment = 0; Segment < 4; Segment++){
          digitalWrite(row[Segment], LOW);
          delay(400);
          digitalWrite(row[Segment], HIGH);
          delay(80);
        }
      }
      digitalWrite(Led1, HIGH);
      i = 5;
    }
  }
}
