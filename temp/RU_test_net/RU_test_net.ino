#include <SPI.h>
#include <Ethernet.h>
byte mac[] = {0xD0, 0xF1, 0xC0, 0xA8, 0x02, 0x42};
IPAddress ip(192, 168, 1, 42);
IPAddress myDns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
EthernetServer server = EthernetServer(404);

String DD1_status = "Close";        
String DD12_status = "Close";
String DD23_status = "Close";
String DD34a_status = "Close";
String DD34b_status = "Close";
String DDT_status = "Close";
String DDdpk_status = "Close";
String DDp1_status = "Close";
String DDp2_status = "Close";
String Grate_status = "Close";
String JACK_status = "Close";
String KEY_status = "Close";
String KEY0_status = "Close";
String KEY1_status = "Close";
String KEY2_status = "Close";
String KEY3_status = "Close";
String KEYBOX_status = "Close";
String NFC0_status = "e024efc0";
String NFC1_status = "e024efc1";
String NFC2_status = "e024efc2";
String NFC3_status = "e024efc3";
String NFC4_status = "e024efc4";
String NFC5_status = "e024efc5";
String NFC6_status = "e024efc6";
String PIR1_status = "Clear";
String PIR2_status = "Clear";
String PIR3_status = "Clear";
String PIR4_status = "Clear";
String PIR5_status = "Clear";
String PIR6_status = "Clear";
String PowerCable_status = "Close";
String TOUCHl_status = "Empty";
String TOUCHr_status = "Empty";
String TruncButton_status = "Close";
String USB_status = "Close";
String UZ0_status = "Empty";
String WIRE_status = "Cut";

void setup() {
  Ethernet.begin(mac, ip, myDns, gateway, subnet);
  server.begin();
}

void loop() {
  boolean firstChar = true;
  String incomingString = "";
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (incomingString.startsWith("DD1=") && (incomingString.length() > 5)) {
            DD1_status = incomingString.substring(4);
          }
          if (incomingString.startsWith("DD12=") && (incomingString.length() > 6)) {
            DD12_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("DD23=") && (incomingString.length() > 6)) {
            DD23_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("DD34a=") && (incomingString.length() > 7)) {
            DD34a_status = incomingString.substring(6);
          }
          if (incomingString.startsWith("DD34b=") && (incomingString.length() > 7)) {
            DD34b_status = incomingString.substring(6);
          }
          if (incomingString.startsWith("DDT=") && (incomingString.length() > 5)) {
            DDT_status = incomingString.substring(4);
          }
          if (incomingString.startsWith("DDdpk=") && (incomingString.length() > 7)) {
            DDdpk_status = incomingString.substring(6);
          }
          if (incomingString.startsWith("DDp1=") && (incomingString.length() > 6)) {
            DDp1_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("DDp2=") && (incomingString.length() > 6)) {
            DDp2_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("Grate=") && (incomingString.length() > 7)) {
            Grate_status = incomingString.substring(6);
          }
          if (incomingString.startsWith("JACK=") && (incomingString.length() > 6)) {
            JACK_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("KEY=") && (incomingString.length() > 5)) {
            KEY_status = incomingString.substring(4);
          }
          if (incomingString.startsWith("KEY0=") && (incomingString.length() > 6)) {
            KEY0_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("KEY1=") && (incomingString.length() > 6)) {
            KEY1_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("KEY2=") && (incomingString.length() > 6)) {
            KEY2_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("KEY3=") && (incomingString.length() > 6)) {
            KEY3_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("KEYBOX=") && (incomingString.length() > 8)) {
            KEYBOX_status = incomingString.substring(7);
          }
          if (incomingString.startsWith("NFC0=") && (incomingString.length() > 6)) {
            NFC0_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("NFC1=") && (incomingString.length() > 6)) {
            NFC1_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("NFC2=") && (incomingString.length() > 6)) {
            NFC2_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("NFC3=") && (incomingString.length() > 6)) {
            NFC3_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("NFC4=") && (incomingString.length() > 6)) {
            NFC4_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("NFC5=") && (incomingString.length() > 6)) {
            NFC5_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("NFC6=") && (incomingString.length() > 6)) {
            NFC6_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("PIR1=") && (incomingString.length() > 6)) {
            PIR1_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("PIR2=") && (incomingString.length() > 6)) {
            PIR2_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("PIR3=") && (incomingString.length() > 6)) {
            PIR3_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("PIR4=") && (incomingString.length() > 6)) {
            PIR4_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("PIR5=") && (incomingString.length() > 6)) {
            PIR5_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("PIR6=") && (incomingString.length() > 6)) {
            PIR6_status = incomingString.substring(5);
          }
          if (incomingString.startsWith("PowerCable=") && (incomingString.length() > 12)) {
            PowerCable_status = incomingString.substring(11);
          }
          if (incomingString.startsWith("TOUCHl=") && (incomingString.length() > 8)) {
            TOUCHl_status = incomingString.substring(7);
          }
          if (incomingString.startsWith("TOUCHr=") && (incomingString.length() > 8)) {
            TOUCHr_status = incomingString.substring(7);
          }
          if (incomingString.startsWith("TruncButton=") && (incomingString.length() > 13)) {
            TruncButton_status = incomingString.substring(12);
          }
          if (incomingString.startsWith("USB=") && (incomingString.length() > 5)) {
            USB_status = incomingString.substring(4);
          }
          if (incomingString.startsWith("UZ0=") && (incomingString.length() > 5)) {
            UZ0_status = incomingString.substring(4);
          }
          if (incomingString.startsWith("WIRE=") && (incomingString.length() > 6)) {
            WIRE_status = incomingString.substring(5);
          }
        delay(5);
        client.stop();
        }
        if (c == 's' && firstChar) {
          client.print("DD1=");
          client.println(DD1_status);
          client.print("DD12=");
          client.println(DD12_status);
          client.print("DD23=");
          client.println(DD23_status);
          client.print("DD34a=");
          client.println(DD34a_status);
          client.print("DD34b=");
          client.println(DD34b_status);
          client.print("DDT=");
          client.println(DDT_status);
          client.print("DDdpk=");
          client.println(DDdpk_status);
          client.print("DDp1=");
          client.println(DDp1_status);
          client.print("DDp2=");
          client.println(DDp2_status);
          client.print("Grate=");
          client.println(Grate_status);
          client.print("JACK=");
          client.println(JACK_status);
          client.print("KEY=");
          client.println(KEY_status);
          client.print("KEY0=");
          client.println(KEY0_status);
          client.print("KEY1=");
          client.println(KEY1_status);
          client.print("KEY2=");
          client.println(KEY2_status);
          client.print("KEY3=");
          client.println(KEY3_status);
          client.print("KEYBOX=");
          client.println(KEYBOX_status);
          client.print("NFC0=");
          client.println(NFC0_status);
          client.print("NFC1=");
          client.println(NFC1_status);
          client.print("NFC2=");
          client.println(NFC2_status);
          client.print("NFC3=");
          client.println(NFC3_status);
          client.print("NFC4=");
          client.println(NFC4_status);
          client.print("NFC5=");
          client.println(NFC5_status);
          client.print("NFC6=");
          client.println(NFC6_status);
          client.print("PIR1=");
          client.println(PIR1_status);
          client.print("PIR2=");
          client.println(PIR2_status);
          client.print("PIR3=");
          client.println(PIR3_status);
          client.print("PIR4=");
          client.println(PIR4_status);
          client.print("PIR5=");
          client.println(PIR5_status);
          client.print("PIR6=");
          client.println(PIR6_status);
          client.print("PowerCable=");
          client.println(PowerCable_status);
          client.print("TOUCHl=");
          client.println(TOUCHl_status);
          client.print("TOUCHr=");
          client.println(TOUCHr_status);
          client.print("TruncButton=");
          client.println(TruncButton_status);
          client.print("USB=");
          client.println(USB_status);
          client.print("UZ0=");
          client.println(UZ0_status);
          client.print("WIRE=");
          client.println(WIRE_status);
            delay(5);
            client.stop();
        }
        if (c != '\r') {
          incomingString.concat(c);
        }
        firstChar = false;
      }
    }
  }
}



