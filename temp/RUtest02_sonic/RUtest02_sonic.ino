#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);
IPAddress myDns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
EthernetServer server = EthernetServer(23);

void setup() {
  pinMode(13, OUTPUT);
  pinMode(12, INPUT);
  digitalWrite(13, LOW);
  Ethernet.begin(mac, ip, myDns, gateway, subnet);
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
        if (c == 'd') {
          digitalWrite(13, HIGH);
          delayMicroseconds(10);
          digitalWrite(13, LOW);
          unsigned int duration = pulseIn(12, HIGH);
          int result = duration / 58;
          client.println(result);
        }
      }
  }
delay(5);
client.stop();
}

