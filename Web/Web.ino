#include <EtherCard.h>
#include <EEPROM.h>
#include <OneWire.h>


byte subnet[] = { 255, 255, 0, 0 };
byte mac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
byte ip[] = { 192, 168, 127, 55 };    
byte gateway[] = { 191, 168, 127, 232 };
byte dns[] = { 8, 8, 8, 8 };


byte Ethernet::buffer[850];

BufferFiller bfill;

OneWire  ds(10);  // on pin 10 (a 4.7K resistor is necessary)
double celsius;

void setup()
{
  Serial.begin(250000);
  
   if (ether.begin(sizeof Ethernet::buffer, mac,10) == 0)
      Serial.println(F("Failed to access Ethernet controller"));  
      Serial.println(F("Ok\n"));  
     // Serial.println(F(mac));
  pinMode(8, OUTPUT);
  digitalWrite(8,1);  
  //Ethernet.begin(mac,myIP);   
  //ether.hisport = 443;
  ether.staticSetup(ip,gateway,dns,subnet);
}
void loop()
{
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  
  if (pos)  // check if valid tcp data is received
    ether.httpServerReply(homePage()); // send web page data
  }
static word homePage() {
  long t = millis() / 1000;
  word h = t / 3600;
  byte m = (t / 60) % 60;
  byte s = t % 60;
  bfill = ether.tcpOffset();
  Serial.println(ether.packetReceive());  
  bfill.emit_p(PSTR(
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html ; charset=utf-8 \r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
//    "<meta http-equiv='refresh' content='10' charset='utf-8'/>"
 //   "<meta charset='utf-8'/>"
    "<html>"
    "<title>Server - первый</title>" 
    "<body>"
    "<h1>Время работы $D$D:$D$D:$D$D</h1>"
    

    "<form action='local' method='post'>"
        "<p><b>LED control</b></p>"
        "<input type='radio' name='led' value='on'>Включить<Br>"
        "<input type='radio' name='led' value='off'>Выключить</p>"
        "<p><input type='submit' value='Обновить'></p>"
    "</form>"
"</html>"
    
    
    ),
      h/10, h%10, m/10, m%10, s/10, s%10);
  return bfill.position();
}

