#include <SPI.h>
#include <Ethernet.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spi11(SPI, 11);
NfcAdapter nfc11 = NfcAdapter(pn532spi11);

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 2, 2);
IPAddress myDns(192, 168, 2, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
EthernetServer server = EthernetServer(23);

void setup() {
    Ethernet.begin(mac, ip, myDns, gateway, subnet);
    server.begin();
    
    Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("NDEF Reader with 3 NFC module");
    nfc11.begin();
}

void loop() {
    Serial.println("\nScan a NFC tag\n");
    if (nfc11.tagPresent())
    {
        NfcTag tag11 = nfc11.read();
        tag11.print();
    }

    EthernetClient client = server.available();
    if (client) {
      if (client.available()) {
        char c = client.read();
        if (c == 's') {
          client.println("Status OK");
        }
      }
  }
delay(5);
client.stop();
delay(1000);
}
