#include <SPI.h>
#include <Ethernet.h>
byte mac[] = {0xD0, 0xF1, 0xC0, 0xA8, 0x02, 0x42};
IPAddress ip(192, 168, 1, 42);
IPAddress myDns(192, 168, 2, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
EthernetServer server = EthernetServer(404);

void setup() {  
  Ethernet.begin(mac, ip, myDns, gateway, subnet);
  server.begin();
}

void loop() {
  //Reply to network requests
  EthernetClient client = server.available();
  if (client) {
    if (client.available()) {
        switch (client.read()) {
          case 's':               //reply with status
            client.println("DD1=Open");
            client.println("DD12=Open");
            client.println("DD23=Open");
            client.println("DD34a=Open");
            client.println("DD34b=Open");
            client.println("DDT=Open");
            client.println("DDdpk=Open");
            client.println("Grate=Open");
            client.println("JACK=Close");
            client.println("KEY=Close");
            client.println("KEY0=0");
            client.println("KEY1=0");
            client.println("KEY2=0");
            client.println("KEY3=0");
            client.println("NFC1=e024efc1");
            client.println("NFC2=e024efc2");
            client.println("NFC3=e024efc3");
            client.println("NFC4=e024efc4");
            client.println("NFC5=e024efc5");
            client.println("NFC6=e024efc6");
            client.println("PIR1=Clear");
            client.println("PIR2=Clear");
            client.println("PIR3=Clear");
            client.println("PIR4=Clear");
            client.println("PIR5=Clear");
            client.println("PIR6=Clear");
            client.println("RfId=e024efc7");
            client.println("TOUCHl=Empty");
            client.println("TOUCHr=Empty");
            client.println("USB=Close");
            client.println("UZ0=Empty");
            client.println("WIRE=Cut");
          break;
          default:
            delay(1);
          break;
        }
     }
  }
  delay(5);
  client.stop();
}



