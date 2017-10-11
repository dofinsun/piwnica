// Arduino Nano!!!!
// for testing RU05 slave part.
  #include <SPI.h>
  #include <PN532_SPI.h>
  #include "PN532.h"
  
  const byte msglength = 4;
  const byte nfcnum = 1;
  
  uint8_t UID1_value [] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t UID2_value [] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t UID3_value [] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  uint8_t success;
  uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

  PN532_SPI pn532spi1(SPI, 8);
  PN532 nfc1(pn532spi1);

  PN532_SPI pn532spi2(SPI, 9);
  PN532 nfc2(pn532spi2);

  PN532_SPI pn532spi3(SPI, 10);
  PN532 nfc3(pn532spi3);

void setup(void) {
  delay(4000);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  uint32_t versiondata;
  
  nfc1.begin();
  versiondata = 0;
  versiondata = nfc1.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board 1");
  }

  Serial.print("Found chip 1 PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  nfc1.SAMConfig();

  nfc2.begin();
  versiondata = 0;
  versiondata = nfc2.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board 2");
  }

  Serial.print("Found chip 2 PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  nfc2.SAMConfig();

  nfc3.begin();
  versiondata = 0;
  versiondata = nfc3.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board 3");
  }

  Serial.print("Found chip 3 PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  nfc3.SAMConfig();

  Serial.println("Waiting for an ISO14443A Card ...");
  //Wire.begin(16);
  //Wire.onReceive(receiveEvent);
  //Wire.onRequest(requestEvent);
}

void loop() {
  
  success = nfc1.readPassiveTargetID(PN532_MIFARE_ISO14443A, UID1_value, &uidLength);
  if (success) {
    readDataFromMiCard1();
  }
  delay(1000);
  success = nfc2.readPassiveTargetID(PN532_MIFARE_ISO14443A, UID2_value, &uidLength);
  if (success) {
    readDataFromMiCard2();
  }
  delay(1000);
  success = nfc3.readPassiveTargetID(PN532_MIFARE_ISO14443A, UID3_value, &uidLength);
  if (success) {
    readDataFromMiCard3();
  }
  delay(1000);
}

void readDataFromMiCard1() {
  Serial.println("Found an ISO14443A card on 1st PN5");
  Serial.print("  UID Length: ");
  Serial.print(uidLength, DEC);
  Serial.println(" bytes");
  Serial.print("  UID Value: ");
  nfc1.PrintHex(UID1_value, uidLength);
  Serial.println("");
  if (uidLength == 4) {
    Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
    Serial.println("Trying to authenticate block 4 with default KEYA value");
    success = nfc1.mifareclassic_AuthenticateBlock(UID1_value, uidLength, 4, 0, keya);
    if (success) {
      Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
      uint8_t data[16];
      // If you want to write something to block 4 to test with, uncomment
      // the following line and this text should be read back in a minute
      // data = { 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0};
      // success = nfc.mifareclassic_WriteDataBlock (4, data);
      success = nfc1.mifareclassic_ReadDataBlock(4, data);
      if (success) {
        Serial.println("Reading Block 4:");
        nfc1.PrintHexChar(data, 16);
        Serial.println("");
      }
      else
      {
        Serial.println("Ooops ... unable to read the requested block.  Try another key?");
      }
    }
    else
    {
      Serial.println("Ooops ... authentication failed: Try another key?");
    }
  }
  if (uidLength == 7) {
    Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");
    Serial.println("Reading page 4");
    uint8_t data[32];
    success = nfc1.mifareultralight_ReadPage (4, data);
    if (success) {
      nfc1.PrintHexChar(data, 4);
      Serial.println("");
    }
    else
    {
      Serial.println("Ooops ... unable to read the requested page!?");
    }
  }
}

void readDataFromMiCard2() {
  Serial.println("Found an ISO14443A card on 2nd PN5");
  Serial.print("  UID Length: ");
  Serial.print(uidLength, DEC);
  Serial.println(" bytes");
  Serial.print("  UID Value: ");
  nfc1.PrintHex(UID2_value, uidLength);
  Serial.println("");
  if (uidLength == 4) {
    Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
    Serial.println("Trying to authenticate block 4 with default KEYA value");
    success = nfc2.mifareclassic_AuthenticateBlock(UID2_value, uidLength, 4, 0, keya);
    if (success) {
      Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
      uint8_t data[16];
      // If you want to write something to block 4 to test with, uncomment
      // the following line and this text should be read back in a minute
      // data = { 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0};
      // success = nfc.mifareclassic_WriteDataBlock (4, data);
      success = nfc2.mifareclassic_ReadDataBlock(4, data);
      if (success) {
        Serial.println("Reading Block 4:");
        nfc2.PrintHexChar(data, 16);
        Serial.println("");
      }
      else
      {
        Serial.println("Ooops ... unable to read the requested block.  Try another key?");
      }
    }
    else
    {
      Serial.println("Ooops ... authentication failed: Try another key?");
    }
  }
  if (uidLength == 7) {
    Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");
    Serial.println("Reading page 4");
    uint8_t data[32];
    success = nfc2.mifareultralight_ReadPage (4, data);
    if (success) {
      nfc2.PrintHexChar(data, 4);
      Serial.println("");
    }
    else
    {
      Serial.println("Ooops ... unable to read the requested page!?");
    }
  }
}

void readDataFromMiCard3() {
  Serial.println("Found an ISO14443A card on 3rd PN5");
  Serial.print("  UID Length: ");
  Serial.print(uidLength, DEC);
  Serial.println(" bytes");
  Serial.print("  UID Value: ");
  nfc3.PrintHex(UID3_value, uidLength);
  Serial.println("");
  if (uidLength == 4) {
    Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
    Serial.println("Trying to authenticate block 4 with default KEYA value");
    success = nfc3.mifareclassic_AuthenticateBlock(UID3_value, uidLength, 4, 0, keya);
    if (success) {
      Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
      uint8_t data[16];
      // If you want to write something to block 4 to test with, uncomment
      // the following line and this text should be read back in a minute
      // data = { 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0};
      // success = nfc.mifareclassic_WriteDataBlock (4, data);
      success = nfc3.mifareclassic_ReadDataBlock(4, data);
      if (success) {
        Serial.println("Reading Block 4:");
        nfc3.PrintHexChar(data, 16);
        Serial.println("");
      }
      else
      {
        Serial.println("Ooops ... unable to read the requested block.  Try another key?");
      }
    }
    else
    {
      Serial.println("Ooops ... authentication failed: Try another key?");
    }
  }
  if (uidLength == 7) {
    Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");
    Serial.println("Reading page 4");
    uint8_t data[32];
    success = nfc3.mifareultralight_ReadPage (4, data);
    if (success) {
      nfc3.PrintHexChar(data, 4);
      Serial.println("");
    }
    else
    {
      Serial.println("Ooops ... unable to read the requested page!?");
    }
  }
}

