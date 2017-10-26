#include <PN532_HSU.h>
#include <PN532.h>
PN532_HSU pn532hsu(Serial1);
PN532 nfc(pn532hsu);
String UID_value, DD1_status, DD2_status, PIR1_status, PIR2_status, USB_status;
const byte ERR_pin = 13;
const byte DD1_pin = 11; //define pin numbers
const byte DD2_pin = 12;
const byte PIR1_pin = 4;
const byte PIR2_pin = 5;
const byte USB_in_pin = 6;
const byte USB_out_pin = 7;
const byte RED1_pin = A0;
const byte RED12_pin = A2;
const byte GR1_pin = A1;
const byte GR12_pin = A3;
const byte DL1_pin = A4;
const byte DL12_pin = A5;

#include <SPI.h>
#include <Ethernet.h>
byte mac[] = {0xD0, 0xF1, 0xC0, 0xA8, 0x02, 0x01};
IPAddress ip(192, 168, 2, 1);
IPAddress myDns(192, 168, 2, 10);
IPAddress gateway(192, 168, 2, 10);
IPAddress subnet(255, 255, 255, 0);
EthernetServer server = EthernetServer(404);

void setup() {
  pinMode(ERR_pin, OUTPUT); //use 13 pin for error notification
  digitalWrite(ERR_pin, LOW); 

  pinMode(DD1_pin, INPUT_PULLUP); //setup pinmode for DD1
  pinMode(DD2_pin, INPUT_PULLUP); 
  pinMode(PIR1_pin, INPUT);       //setup pinmode for IR1,IR2
  pinMode(PIR2_pin, INPUT);
  pinMode(USB_in_pin, INPUT);     // setup USB ports
  pinMode(USB_out_pin, OUTPUT);
  pinMode(RED1_pin, OUTPUT);      //Led`s
  pinMode(GR1_pin, OUTPUT);
  pinMode(RED12_pin, OUTPUT);
  pinMode(GR12_pin, OUTPUT);
  digitalWrite(RED1_pin, HIGH);
  digitalWrite(GR1_pin, HIGH);
  digitalWrite(RED12_pin, HIGH);
  digitalWrite(GR12_pin, HIGH);
  pinMode(DL1_pin, OUTPUT);       //Door locks
  pinMode(DL12_pin, OUTPUT);
  digitalWrite(DL1_pin, HIGH);
  digitalWrite(DL12_pin, HIGH);
  
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    digitalWrite(13, HIGH); //meet exception, light led and halt
    while (1);
  }
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();
  
  Ethernet.begin(mac, ip, myDns, gateway, subnet);
  server.begin();
}

void loop() {
  boolean success, drum;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
  //read NFC
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  if (success) {
    UID_value = "";
    for (uint8_t i=0; i < uidLength; i++) {
      UID_value += String(uid[i], HEX);
    }
  }

  //Read door status DD1, DD2
  drum = digitalRead(DD1_pin);
  if (drum) {
    DD1_status = "Open";
  } else {
    DD1_status = "Close";
  }
  drum = digitalRead(DD2_pin);
  if (drum) {
    DD2_status = "Open";
  } else {
    DD2_status = "Close";
  }

  //Read movement PIR1, PIR2
  drum = digitalRead(PIR1_pin);
  if (drum) {
    PIR1_status = "Move";
  } else {
    PIR1_status = "Clear";
  }
  drum = digitalRead(PIR2_pin);
  if (drum) {
    PIR2_status = "Move";
  } else {
    PIR2_status = "Clear";
  }

  //Read USB status
  success = true;
  for (byte i = 0; i < 10; i++) {
    digitalWrite(USB_out_pin, HIGH);
    delay(1);
    drum = digitalRead(USB_in_pin);  
    if (!drum) {
      success = false;
    }
    digitalWrite(USB_out_pin, LOW);
    delay(1);
    drum = digitalRead(USB_in_pin);
    if (drum) {
      success = false;
    }
  }
  if (success) {
    USB_status = "Open";
  } else {
    USB_status = "Close";
  }

  //Reply to network requests
  EthernetClient client = server.available();
  if (client) {
    if (client.available()) {
        switch (client.read()) {
          case 's':               //reply with status
            client.println("");
            client.print("NFC0=");
            client.println(UID_value);
            client.print("DD1=");
            client.println(DD1_status);
            client.print("DD12=");
            client.println(DD2_status);
            client.print("PIR1=");
            client.println(PIR1_status);
            client.print("PIR2=");
            client.println(PIR2_status);
            client.print("USB=");
            client.println(USB_status);
            UID_value = "";
            break;
          case 'q':               //unlock DL1
            digitalWrite(GR1_pin, HIGH);
            digitalWrite(RED1_pin, LOW);
            digitalWrite(DL1_pin, HIGH);
            break;
          case 'w':               //lock DL1
            digitalWrite(GR1_pin, LOW);
            digitalWrite(RED1_pin, HIGH);
            digitalWrite(DL1_pin, LOW);
            break;
          case 'e':               //unlock DL12
            digitalWrite(GR12_pin, HIGH);
            digitalWrite(RED12_pin, LOW);
            digitalWrite(DL12_pin, HIGH);
            break;
          case 'r':               //lock DL12
            digitalWrite(GR12_pin, LOW);
            digitalWrite(RED12_pin, HIGH);
            digitalWrite(DL12_pin, LOW);
            break;
        }
     }
  }
  delay(5);
  client.stop();
}



