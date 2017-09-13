String DD23_status, DDdpk_status, PIR3_status, PIR4_status, JACK_status, KEY_status;
int ERR_pin = 13;
int DD23_pin = 12;
int DDdpk_pin = 11;
int PIR3_pin = A2;
int PIR4_pin = A3;
int RED23_pin = A0;
int GR23_pin = A1;
int DL23_pin = A4;
int DLdpk_pin = A5;
const int LED_pin[3] = { 0, 1, 2 };
const int JACK_in_pin[3] = { 3, 5, 7 };
const int JACK_out_pin[3] = { 4, 6, 8 };
int KEY_pin = 9;

#include <SPI.h>
#include <Ethernet.h>
byte mac[] = {0xD0, 0xF1, 0xC0, 0xA8, 0x02, 0x03};
IPAddress ip(192, 168, 2, 3);
IPAddress myDns(192, 168, 2, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
EthernetServer server = EthernetServer(404);


void setup() {
  pinMode(ERR_pin, OUTPUT); //use 13 pin for error notification
  digitalWrite(ERR_pin, LOW);

  pinMode(DD23_pin, INPUT_PULLUP);
  pinMode(DDdpk_pin, INPUT_PULLUP);
  pinMode(PIR3_pin, INPUT);
  pinMode(PIR4_pin, INPUT);
  pinMode(RED23_pin, OUTPUT);
  pinMode(GR23_pin, OUTPUT);
  digitalWrite(RED23_pin, HIGH);
  digitalWrite(GR23_pin, HIGH);
  pinMode(DL23_pin, OUTPUT);
  pinMode(DLdpk_pin, OUTPUT);
  digitalWrite(DL23_pin, LOW);
  digitalWrite(DLdpk_pin, LOW);
  for (int thisPin = 0; thisPin < 3; thisPin++) {
    pinMode(LED_pin[thisPin], OUTPUT);
    pinMode(JACK_in_pin[thisPin], INPUT);
    pinMode(JACK_out_pin[thisPin], OUTPUT);
  }
  pinMode(KEY_pin, INPUT_PULLUP);

  Ethernet.begin(mac, ip, myDns, gateway, subnet);
  server.begin();
}

void loop() {
  boolean drum;
  boolean patch_count[3] = { true, true, true };
  
  
  drum = digitalRead(DD23_pin);
  if (drum) {
    DD23_status = "Open";
  } else {
    DD23_status = "Close";
  }
  
  drum = digitalRead(DDdpk_pin);
  if (drum) {
    DDdpk_status = "Open";
  } else {
    DDdpk_status = "Close";
  }

  drum = digitalRead(PIR3_pin);
  if (drum) {
    PIR3_status = "Move";
  } else {
    PIR3_status = "Clear";
  }

  drum = digitalRead(PIR4_pin);
  if (drum) {
    PIR4_status = "Move";
  } else {
    PIR4_status = "Clear";
  }

  for (int thisPin = 0; thisPin < 3; thisPin++) {
    for (byte i = 0; i < 10; i++) {
      digitalWrite(JACK_out_pin[thisPin], HIGH);
      delay(1);
      drum = digitalRead(JACK_in_pin[thisPin]);  
      if (!drum) {
        patch_count[thisPin] = false;
      }
      digitalWrite(JACK_out_pin[thisPin], LOW);
      delay(1);
      drum = digitalRead(JACK_in_pin[thisPin]);
      if (drum) {
        patch_count[thisPin] = false;
      }
    }
    if ( patch_count[thisPin] ) {
      digitalWrite(LED_pin[thisPin], HIGH);
    } else {
      digitalWrite(LED_pin[thisPin], LOW);
    }
  }
  if ( patch_count[0] && patch_count[1] && patch_count[2] ) {
    JACK_status = "Open";
  } else {
    JACK_status = "Close";
  }

  drum = digitalRead(KEY_pin);
  if (drum) {
    KEY_status = "Close";
  } else {
    KEY_status = "Open";
  }

  EthernetClient client = server.available();
  if (client) {
    if (client.available()) {
        switch (client.read()) {
          case 's':               //reply with status
            client.println("");
            client.print("DD23=");
            client.println(DD23_status);
            client.print("DDdpk=");
            client.println(DDdpk_status);
            client.print("PIR3=");
            client.println(PIR3_status);
            client.print("PIR4=");
            client.println(PIR4_status);
            client.print("JACK=");
            client.println(JACK_status);
            client.print("KEY=");
            client.println(KEY_status);
            break;
          case 'q':               //unlock DL23
            digitalWrite(GR23_pin, HIGH);
            digitalWrite(RED23_pin, LOW);
            digitalWrite(DL23_pin, LOW);
            break;
          case 'w':               //lock DL23
            digitalWrite(GR23_pin, LOW);
            digitalWrite(RED23_pin, HIGH);
            digitalWrite(DL23_pin, HIGH);
            break;
          case 'e':               //unlock DLdpk
            digitalWrite(DLdpk_pin, LOW);
            break;
          case 'r':               //lock DLdpk
            digitalWrite(DLdpk_pin, HIGH);
            break;
        }
     }
  }
  delay(5);
  client.stop();
}
