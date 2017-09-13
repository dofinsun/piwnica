#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 2, 2);
IPAddress myDns(192, 168, 2, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
EthernetServer server = EthernetServer(23);

void setup() {
//  Serial.begin(9600);
//  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
//  }
//  Serial.println(Ethernet.begin(mac, ip, myDns, gateway, subnet));
  
  server.begin();
}

void loop() {
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
}



