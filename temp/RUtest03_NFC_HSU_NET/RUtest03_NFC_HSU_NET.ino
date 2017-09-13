#include <PN532_HSU.h>
#include <PN532.h>
PN532_HSU pn532hsu(Serial1);
PN532 nfc(pn532hsu);
String UID_value, DD1_status, DD2_status, PIR1_status, PIR2_status, USB_status;
int ERR_pin = 13;
int DD1_pin = 11; //define pin numbers
int DD2_pin = 12;
int PIR1_pin = 4;
int PIR2_pin = 5;
int USB_in_pin = 6;
int USB_out_pin = 7;
int RED1_pin = A0;
int RED12_pin = A2;
int GR1_pin = A1;
int GR12_pin = A3;
int DL1_pin = A4;
int DL12_pin = A5;

#include <SPI.h>
#include <Ethernet.h>
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 2, 2);
IPAddress myDns(192, 168, 2, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
EthernetServer server = EthernetServer(23);

void setup() {
  pinMode(ERR_pin, OUTPUT); //use 13 pin for error notification
  digitalWrite(ERR_pin, HIGH);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  digitalWrite(ERR_pin, LOW); 
  Serial.println("Program started.");

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
    Serial.println("Unable to find NDF module");
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
    Serial.println("Get new rfid");
    for (uint8_t i=0; i < uidLength; i++) {
      UID_value += String(uid[i], HEX);
    }
    Serial.print("NEW RfId = ");   //print corrent status to serial
    Serial.println(UID_value);
  }
  Serial.print("Current RfId = ");   //print corrent status to serial
  Serial.println(UID_value);

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
  Serial.print("DD1 = ");     //print corrent status to serial
  Serial.println(DD1_status);
  Serial.print("DD2 = ");
  Serial.println(DD2_status);

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
  Serial.print("PIR1 = ");      //print current status to serial
  Serial.println(PIR1_status);
  Serial.print("PIR2 = ");
  Serial.println(PIR2_status);

  //Read USB status
  success = true;
  for (byte i = 0; i < 10; i++) {
    digitalWrite(USB_out_pin, HIGH);
    delay(1);
    drum = digitalRead(USB_in_pin);
    
//    Serial.print("drum_H = ");  //print current status to serial
//    Serial.println(drum);
    
    if (!drum) {
      success = false;
//      Serial.println("fail");
    }
    digitalWrite(USB_out_pin, LOW);
    delay(1);
    drum = digitalRead(USB_in_pin);
    
//    Serial.print("drum_L= ");  //print current status to serial
//    Serial.println(drum);
    
    if (drum) {
      success = false;
//      Serial.println("fail");
    }
  }
  if (success) {
    USB_status = "Open";
  } else {
    USB_status = "Close";
  }
  Serial.print("USB = ");     //print corrent status to serial
  Serial.println(USB_status);

  //Reply to network requests
  EthernetClient client = server.available();
  if (client) {
    if (client.available()) {
        switch (client.read()) {
          case 's':               //reply with status
            client.println("");
            client.print("RfId=");
            client.println(UID_value);
            client.print("DD1=");
            client.println(DD1_status);
            client.print("DD2=");
            client.println(DD2_status);
            client.print("PIR1=");
            client.println(PIR1_status);
            client.print("PIR2=");
            client.println(PIR2_status);
            client.print("USB=");
            client.println(USB_status);
            client.println("Status OK");
            UID_value = "";
            break;
          case 'q':               //unlock D1
            digitalWrite(GR1_pin, HIGH);
            digitalWrite(RED1_pin, LOW);
            digitalWrite(DL1_pin, LOW);
            break;
          case 'w':               //lock D1
            digitalWrite(GR1_pin, LOW);
            digitalWrite(RED1_pin, HIGH);
            digitalWrite(DL1_pin, HIGH);
            break;
          case 'e':               //unlock D12
            digitalWrite(GR12_pin, HIGH);
            digitalWrite(RED12_pin, LOW);
            digitalWrite(DL12_pin, LOW);
            break;
          case 'r':               //lock D12
            digitalWrite(GR12_pin, LOW);
            digitalWrite(RED12_pin, HIGH);
            digitalWrite(DL12_pin, HIGH);
            break;
        }
     }
  }
  delay(5);
  client.stop();
  delay(1000);
}



