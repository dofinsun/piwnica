#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>

const byte ERR_pin = 13;
String PIR6_status, Wire_cut;
String NFC_UID[3] = {"", "", ""};
String NFC_REP[3] = {"NFC4=", "NFC5=", "NFC6="};
unsigned long prev_time = 0;
unsigned long curr_time = 0;
unsigned long delta_time = 0;
const byte PIR6_pin = 12;
const byte DLp1_pin = A0;
const byte DLp2_pin = A1;
const byte DLven_pin = A2;
const byte DLL0_pin = A5;
const byte DLL1_pin = A4;
const byte DLL2_pin = A3;
const byte Wire_cut_in_pin = 4;
const byte Wire_cut_out_pin = 5;

byte mac[] = {0xD0, 0xF1, 0xC0, 0xA8, 0x02, 0x07};
IPAddress ip(192, 168, 2, 7);
IPAddress myDns(192, 168, 2, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
EthernetServer server = EthernetServer(404);

void setup() {
  pinMode(ERR_pin, OUTPUT); //use 13 pin for error notification
  digitalWrite(ERR_pin, LOW);

  pinMode(PIR6_pin, INPUT);
  pinMode(DLp1_pin, OUTPUT);
  pinMode(DLp2_pin, OUTPUT);
  pinMode(DLven_pin, OUTPUT);
  pinMode(DLL0_pin, OUTPUT);
  pinMode(DLL1_pin, OUTPUT);
  pinMode(DLL2_pin, OUTPUT);
  digitalWrite(DLp1_pin, HIGH);
  digitalWrite(DLp2_pin, HIGH);
  digitalWrite(DLven_pin, HIGH);
  digitalWrite(DLL0_pin, HIGH);
  digitalWrite(DLL1_pin, HIGH);
  digitalWrite(DLL2_pin, HIGH);
  pinMode(Wire_cut_in_pin, INPUT);
  pinMode(Wire_cut_out_pin, OUTPUT);
  digitalWrite(Wire_cut_out_pin, LOW);

  Wire.begin();
  
  Ethernet.begin(mac, ip, myDns, gateway, subnet);
  server.begin();
}

void loop() {
  boolean drum;
  
  //get NFC status by i2c once per second
  curr_time = millis();
  delta_time = curr_time - prev_time;
  if (delta_time > 1000) {
    for (byte i = 0; i < 3; i++) {
      NFC_UID[i] = "";
      Wire.beginTransmission(16);
      Wire.write(i);
      Wire.endTransmission();
      delay(3);
      Wire.requestFrom(16, 4);
      while (Wire.available()) {
        byte c = Wire.read();
        NFC_UID[i] += String(c,HEX);
      }
      delay(3);
    }
    prev_time = curr_time;
  }

  //Read PIR status
  drum = digitalRead(PIR6_pin);
  if (drum) {
    PIR6_status = "Move";
  } else {
    PIR6_status = "Clear";
  }

  //Check wire status
  boolean wire_stat = true;
  for (byte i = 0; i < 10; i++) {
    digitalWrite(Wire_cut_out_pin, HIGH);
    delay(1);
    drum = digitalRead(Wire_cut_in_pin);  
    if (!drum) {
      wire_stat = false;
    }
    digitalWrite(Wire_cut_out_pin, LOW);
    delay(1);
    drum = digitalRead(Wire_cut_in_pin);
    if (drum) {
      wire_stat = false;
    }
  }
  if (wire_stat) {
    Wire_cut = "Intact";
  } else {
    Wire_cut = "Cut";
  }
  
  
  //Reply to network requests
  EthernetClient client = server.available();
  if (client) {
    if (client.available()) {
        switch (client.read()) {
          case 's': //reply with status PIR6, Wire_cut, NFC`s;
            client.println("");
            client.print("PIR6=");
            client.println(PIR6_status);
            client.print("WIRE=");
            client.println(Wire_cut);
            for (byte i = 0; i < 3; i++) {
              client.print(NFC_REP[i]);
              client.println(NFC_UID[i]);
            }
            break;
          case 'q':               //unlock DLp1
            digitalWrite(DLp1_pin, HIGH);
            break;
          case 'w':               //lock DLp1
            digitalWrite(DLp1_pin, LOW);
            break;
          case 'e':               //unlock DLp2
            digitalWrite(DLp2_pin, HIGH);
            break;
          case 'r':               //lock DLp2
            digitalWrite(DLp2_pin, LOW);
            break;
          case 't':               //unlock DLven
            digitalWrite(DLven_pin, HIGH);
            break;
          case 'y':               //lock DLven
            digitalWrite(DLven_pin, LOW);
            break;
          case 'u':               //unlock DLL0
            digitalWrite(DLL0_pin, HIGH);
            break;
          case 'i':               //lock DLL0
            digitalWrite(DLL0_pin, LOW);
            break;
          case 'a':               //unlock DLL1
            digitalWrite(DLL1_pin, HIGH);
            break;
          case 'd':               //lock DLL1
            digitalWrite(DLL1_pin, LOW);
            break;
          case 'f':               //unlock DLL2
            digitalWrite(DLL2_pin, HIGH);
            break;
          case 'g':               //lock DLL2
            digitalWrite(DLL2_pin, LOW);
            break;
        }
     }
  }
  delay(5);
  client.stop();
}
