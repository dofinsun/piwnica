  // Arduino Nano!!!!
  #include <Wire.h>
  #include <SPI.h>
  #include <PN532_SPI.h>
  #include "PN532.h"
  
  uint8_t ERR_pin = 13;
  uint8_t msglength = 4;
  uint8_t nfcnum = 0;
    
  uint8_t UID0_value [] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t UID1_value [] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t UID2_value [] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  PN532_SPI pn532spi0(SPI, 8);
  PN532 nfc0(pn532spi0);

  PN532_SPI pn532spi1(SPI, 9);
  PN532 nfc1(pn532spi1);

  PN532_SPI pn532spi2(SPI, 10);
  PN532 nfc2(pn532spi2);

void setup(void) {
  pinMode(ERR_pin, OUTPUT);
  digitalWrite(ERR_pin, LOW);
  
  uint32_t versiondata;
  
  nfc0.begin();
  versiondata = 0;
  versiondata = nfc0.getFirmwareVersion();
  if (! versiondata) {
    digitalWrite(ERR_pin, HIGH);
    while (1); // halt on error
  }
  nfc0.SAMConfig();

  nfc1.begin();
  versiondata = 0;
  versiondata = nfc1.getFirmwareVersion();
  if (! versiondata) {
    digitalWrite(ERR_pin, HIGH);
    while (1);
  }
  nfc1.SAMConfig();

  nfc2.begin();
  versiondata = 0;
  versiondata = nfc2.getFirmwareVersion();
  if (! versiondata) {
    digitalWrite(ERR_pin, HIGH);
    while (1);
  }
  nfc2.SAMConfig();
  
  Wire.begin(16);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
  nfc0.readPassiveTargetID(PN532_MIFARE_ISO14443A, UID0_value, &uidLength);
  delay(300);
  nfc1.readPassiveTargetID(PN532_MIFARE_ISO14443A, UID1_value, &uidLength);
  delay(300);
  nfc2.readPassiveTargetID(PN532_MIFARE_ISO14443A, UID2_value, &uidLength);
  delay(300);
}

void receiveEvent(int howMany) {
  while (Wire.available()) {
    nfcnum = Wire.read();
  }
}

void requestEvent() {
  switch (nfcnum) {
    case 0:
      Wire.write(UID0_value, msglength);
      for (int i = 0; i < 7; i++) {
        UID0_value[i]=0;
      }
      break;
    case 1:
      Wire.write(UID1_value, msglength);
      for (int i = 0; i < 7; i++) {
        UID1_value[i]=0;
      }
      break;
    case 2:
      Wire.write(UID2_value, msglength);
      for (int i = 0; i < 7; i++) {
        UID2_value[i]=0;
      }
      break;
    default: 
      ;
    break;
  }
}

