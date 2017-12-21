String DD34a_status, DD34b_status, UZ0_status, PowerCable_status, TruncButton_status;
const byte RED34a_pin = A0;
const byte GR34a_pin = A1;
const byte RED34b_pin = A2;
const byte GR34b_pin = A3;
const byte DL34a_pin = A4;
const byte DL34b_pin = A5;
const byte Trig_pin = 2;
const byte Echo_pin = 3;
const byte DLcomBox_pin = 4;
const byte DD34b_pin = 5;
const byte DD34a_pin = 6;
const byte PowerCable_pin = 7;
const byte TruncButton_pin = 8;
const byte TruncLed_pin = 9;
const byte RedAlert_pin = 11;
const byte ERR_pin = 13;
unsigned int doorway = 3800; // empty doorway

#include <SPI.h>
#include <Ethernet.h>
byte mac[] = {0xD0, 0xF1, 0xC0, 0xA8, 0x02, 0x03};
IPAddress ip(192, 168, 2, 3);
IPAddress myDns(192, 168, 2, 10);
IPAddress gateway(192, 168, 2, 10);
IPAddress subnet(255, 255, 255, 0);
EthernetServer server = EthernetServer(404);

void setup() {
  pinMode(ERR_pin, OUTPUT); //use 13 pin for error notification
  digitalWrite(ERR_pin, LOW);
  pinMode(RED34a_pin, OUTPUT);
  pinMode(GR34a_pin, OUTPUT);
  digitalWrite(RED34a_pin, HIGH);
  digitalWrite(GR34a_pin, HIGH);
  pinMode(RED34b_pin, OUTPUT);
  pinMode(GR34b_pin, OUTPUT);
  digitalWrite(RED34b_pin, HIGH);
  digitalWrite(GR34b_pin, HIGH);
  pinMode(DL34a_pin, OUTPUT);
  pinMode(DL34b_pin, OUTPUT);
  digitalWrite(DL34a_pin, HIGH);
  digitalWrite(DL34b_pin, HIGH);
  pinMode(Trig_pin, OUTPUT);
  digitalWrite(Trig_pin, LOW);
  pinMode(Echo_pin, INPUT);
  pinMode(DD34b_pin, INPUT_PULLUP);
  pinMode(DD34a_pin, INPUT_PULLUP);
  pinMode(PowerCable_pin, INPUT_PULLUP);
  pinMode(DLcomBox_pin, OUTPUT);
  digitalWrite(DLcomBox_pin, HIGH);
  pinMode(TruncButton_pin, INPUT_PULLUP);
  pinMode(TruncLed_pin, OUTPUT);
  digitalWrite(TruncLed_pin, HIGH);
  pinMode(RedAlert_pin, OUTPUT);
  digitalWrite(RedAlert_pin, HIGH);
  
  Ethernet.begin(mac, ip, myDns, gateway, subnet);
  server.begin();
}

void loop() {
  boolean drum;
  unsigned int duration;

  drum = digitalRead(DD34a_pin);
  if (drum) {
    DD34a_status = "Open";
  } else {
    DD34a_status = "Close";
  }

  drum = digitalRead(DD34b_pin);
  if (drum) {
    DD34b_status = "Open";
  } else {
    DD34b_status = "Close";
  }

  digitalWrite(Trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig_pin, LOW);
  duration = pulseIn(Echo_pin, HIGH);
  if (duration > doorway) {
    UZ0_status = "Empty";
  } else {
    UZ0_status = "Detect";
  }
  
  drum = digitalRead(PowerCable_pin);
  if (drum) {
    PowerCable_status = "Close";
  } else {
    PowerCable_status = "Open";
  }

    drum = digitalRead(TruncButton_pin);
  if (drum) {
    TruncButton_status = "Close";
  } else {
    TruncButton_status = "Open";
  }
  
  EthernetClient client = server.available();
  if (client) {
    if (client.available()) {
        switch (client.read()) {
          case 's':               //reply with status
            client.println("");
            client.print("DD34a=");
            client.println(DD34a_status);
            client.print("DD34b=");
            client.println(DD34b_status);
            client.print("UZ0=");
            client.println(UZ0_status);
            client.print("PowerCable=");
            client.println(PowerCable_status);
            client.print("TruncButton=");
            client.println(TruncButton_status);
            break;
          case 'q':               //unlock DL34a
            digitalWrite(GR34a_pin, HIGH);
            digitalWrite(RED34a_pin, LOW);
            digitalWrite(DL34a_pin, HIGH);
            break;
          case 'w':               //lock DL34a
            digitalWrite(GR34a_pin, LOW);
            digitalWrite(RED34a_pin, HIGH);
            digitalWrite(DL34a_pin, LOW);
            break;
          case 'e':               //unlock DL34b
            digitalWrite(GR34b_pin, HIGH);
            digitalWrite(RED34b_pin, LOW);
            digitalWrite(DL34b_pin, HIGH);
            break;
          case 'r':               //lock DL34b
            digitalWrite(GR34b_pin, LOW);
            digitalWrite(RED34b_pin, HIGH);
            digitalWrite(DL34b_pin, LOW);
            break;
          case 't':               //unlock DLComBox
            digitalWrite(DLcomBox_pin, HIGH);
            break;
          case 'y':               //lock DLComBox
            digitalWrite(DLcomBox_pin, LOW);
            break;
          case 'a':               //on TruncLed
            digitalWrite(TruncLed_pin, HIGH);
            break;
          case 'd':               //off TruncLed
            digitalWrite(TruncLed_pin, LOW);
            break;
          case 'f':               //unlock RedAlert
            digitalWrite(RedAlert_pin, HIGH);
            break;
          case 'g':               //lock RedAlert
            digitalWrite(RedAlert_pin, LOW);
            break;
        }
     }
  }
  delay(5);
  client.stop();
}















