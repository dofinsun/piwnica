String DD34a_status, DD34b_status, UZ0_status;
int ERR_pin = 13;
int DD34a_pin = 5;
int DD34b_pin = 4;
int DL34a_pin = A4;
int DL34b_pin = A5;
int RED34a_pin = A0;
int GR34a_pin = A1;
int RED34b_pin = A2;
int GR34b_pin = A3;
int Trig_pin = 2;
int Echo_pin = 3;
unsigned int doorway = 3500; // empty doorway

#include <SPI.h>
#include <Ethernet.h>
byte mac[] = {0xD0, 0xF1, 0xC0, 0xA8, 0x02, 0x04};
IPAddress ip(192, 168, 2, 4);
IPAddress myDns(192, 168, 2, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
EthernetServer server = EthernetServer(404);

void setup() {
  pinMode(ERR_pin, OUTPUT); //use 13 pin for error notification
  digitalWrite(ERR_pin, LOW);

  pinMode(DD34a_pin, INPUT_PULLUP);
  pinMode(DD34b_pin, INPUT_PULLUP);
  pinMode(DL34a_pin, OUTPUT);
  pinMode(DL34b_pin, OUTPUT);
  digitalWrite(DL34a_pin, LOW);
  digitalWrite(DL34b_pin, LOW);
  pinMode(RED34a_pin, OUTPUT);
  pinMode(GR34a_pin, OUTPUT);
  digitalWrite(RED34a_pin, HIGH);
  digitalWrite(GR34a_pin, HIGH);
  pinMode(RED34b_pin, OUTPUT);
  pinMode(GR34b_pin, OUTPUT);
  digitalWrite(RED34b_pin, HIGH);
  digitalWrite(GR34b_pin, HIGH);
  pinMode(Echo_pin, INPUT);
  pinMode(Trig_pin, OUTPUT);
  digitalWrite(Trig_pin, LOW);

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
            break;
          case 'q':               //unlock DL34a
            digitalWrite(GR34a_pin, HIGH);
            digitalWrite(RED34a_pin, LOW);
            digitalWrite(DL34a_pin, LOW);
            break;
          case 'w':               //lock DL34a
            digitalWrite(GR34a_pin, LOW);
            digitalWrite(RED34a_pin, HIGH);
            digitalWrite(DL34a_pin, HIGH);
            break;
          case 'e':               //unlock DL34b
            digitalWrite(GR34b_pin, HIGH);
            digitalWrite(RED34b_pin, LOW);
            digitalWrite(DL34b_pin, LOW);
            break;
          case 'r':               //lock DL34b
            digitalWrite(GR34b_pin, LOW);
            digitalWrite(RED34b_pin, HIGH);
            digitalWrite(DL34b_pin, HIGH);
            break;
        }
     }
  }
  delay(5);
  client.stop();
}















