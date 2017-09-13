#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spi10(SPI, 10);
NfcAdapter nfc10 = NfcAdapter(pn532spi10);
PN532_SPI pn532spi11(SPI, 11);
NfcAdapter nfc11 = NfcAdapter(pn532spi11);
PN532_SPI pn532spi12(SPI, 12);
NfcAdapter nfc12 = NfcAdapter(pn532spi12);

void setup(void) {
    Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("NDEF Reader with 3 NFC module");
    nfc10.begin();
    nfc11.begin();
    nfc12.begin();
}

void loop(void) {
    Serial.println("\nScan a NFC tag\n");
    if (nfc10.tagPresent())
    {
        NfcTag tag = nfc10.read();
        tag.print();
    }
    if (nfc11.tagPresent())
    {
        NfcTag tag = nfc11.read();
        tag.print();
    }

    if (nfc12.tagPresent())
    {
        NfcTag tag = nfc12.read();
        tag.print();
    }
    delay(5000);
}
