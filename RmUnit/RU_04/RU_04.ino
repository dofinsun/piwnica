String DDT_status,KEY0_status,KEY1_status,KEY2_status,KEY3_status;
const byte DLW_pin = A4;
const byte DLT_pin = A5;
const byte DDT_pin = A3;
const byte RED_LED_pin[4] = { A0, A1, A2, 13 };
const byte GREEN_LED_pin[4] = { 0, 1, 2, 12 };
const byte JACK_in_pin[4] = { 3, 5, 7, 9 };
const byte JACK_out_pin[4] = { 4, 6, 8, 11 };
boolean patch_enable[4] = { true, true, true, true };

#include <SPI.h>
#include <Ethernet.h>
byte mac[] = {0xD0, 0xF1, 0xC0, 0xA8, 0x02, 0x04};
IPAddress ip(192, 168, 2, 4);
IPAddress myDns(192, 168, 2, 10);
IPAddress gateway(192, 168, 2, 10);
IPAddress subnet(255, 255, 255, 0);
EthernetServer server = EthernetServer(404);

void setup() {
  pinMode(DLW_pin, OUTPUT);
  pinMode(DLT_pin, OUTPUT);
  digitalWrite(DLW_pin, HIGH);
  digitalWrite(DLT_pin, HIGH);
  pinMode(DDT_pin, INPUT_PULLUP);
   for (char thisPin = 0; thisPin <=3; thisPin++) {
    pinMode(RED_LED_pin[thisPin], OUTPUT);
    pinMode(GREEN_LED_pin[thisPin], OUTPUT);
    pinMode(JACK_in_pin[thisPin], INPUT);
    pinMode(JACK_out_pin[thisPin], OUTPUT);
  }
  
  Ethernet.begin(mac, ip, myDns, gateway, subnet);
  server.begin();
}

void loop() {
  boolean drum;
  boolean patch_count[4] = { true, true, true, true };

  drum = digitalRead(DDT_pin);
  if (drum) {
    DDT_status = "Open";
  } else {
    DDT_status = "Close";
  }
  
  for (int thisPin = 0; thisPin <=3; thisPin++) {
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
    if (patch_enable[thisPin]){
      if ( patch_count[thisPin] ) {
        digitalWrite(GREEN_LED_pin[thisPin], HIGH);
        digitalWrite(RED_LED_pin[thisPin], LOW);
      } else {
        digitalWrite(GREEN_LED_pin[thisPin], LOW);
        digitalWrite(RED_LED_pin[thisPin], HIGH);
      }
    } else {
      digitalWrite(GREEN_LED_pin[thisPin], LOW);
      digitalWrite(RED_LED_pin[thisPin], LOW);
    }
  }

  if (patch_count[0]) {
    KEY0_status = "Open";
  } else {
    KEY0_status = "Close";
  }
  if (patch_count[1]) {
    KEY1_status = "Open";
  } else {
    KEY1_status = "Close";
  }
  if (patch_count[2]) {
    KEY2_status = "Open";
  } else {
    KEY2_status = "Close";
  }
  if (patch_count[3]) {
    KEY3_status = "Open";
  } else {
    KEY3_status = "Close";
  }
  
  EthernetClient client = server.available();
  if (client) {
    if (client.available()) {
        switch (client.read()) {
          case 's':               //reply with status
            client.println("");
            client.print("DDT=");
            client.println(DDT_status);
            client.print("KEY0=");
            client.println(KEY0_status);
            client.print("KEY1=");
            client.println(KEY1_status);
            client.print("KEY2=");
            client.println(KEY2_status);
            client.print("KEY3=");
            client.println(KEY3_status);
            break;
          case 'q':               //unlock DLW
            digitalWrite(DLW_pin, HIGH);
            break;
          case 'w':               //lock DLW
            digitalWrite(DLW_pin, LOW);
            break;
          case 'e':               //unlock DLT
            digitalWrite(DLT_pin, HIGH);
            break;
          case 'r':               //lock DLT
            digitalWrite(DLT_pin, LOW);
            break;
          case 'a':               //decrease Key count
            for (int thisPin = 0; thisPin <=3; thisPin++) {
              if (!patch_enable[thisPin]) {
                patch_enable[thisPin - 1] = false;
              }
            }
            patch_enable[3] = false;
            break;
          case 'd':               //reset Key count
            for (int thisPin = 0; thisPin <=3; thisPin++) {
              patch_enable[thisPin] = true;
            }
            break;
        }
     }
  }
  delay(5);
  client.stop();
}
