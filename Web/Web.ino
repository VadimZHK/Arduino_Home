#include <EtherCard.h>
#include <EEPROM.h>
#include <OneWire.h>


byte subnet[] = { 255, 255, 0, 0 };
byte mac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };
byte ip[] = { 192, 168, 127, 55 };
byte gateway[] = { 191, 168, 127, 232 };
byte dns[] = { 8, 8, 8, 8 };
int led;


const char okHeader[] PROGMEM =
  "HTTP/1.0 200 OK\r\n"
  "Content-Type: text/html; charset=utf-8\r\n"
  "Pragma: no-cache\r\n"
  ;


byte Ethernet::buffer[850];

BufferFiller bfill;

//OneWire  ds(3);  // on pin 10 (a 4.7K resistor is necessary)
double celsius;

void setup()
{
  Serial.begin(250000);

  if (ether.begin(sizeof Ethernet::buffer, mac, 10) == 0)
    Serial.println(F("Failed to access Ethernet controller"));

  Serial.println(F("Ok\n"));
  // Serial.println(F(mac));
  //  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);//LOW);//
  //  digitalWrite(8,1);
  //Ethernet.begin(mac,myIP);
  //ether.hisport = 443;
  ether.staticSetup(ip, gateway, dns, subnet);
  ether.printIp("My IP: ", ether.myip);
  ether.printIp("Netmask: ", ether.netmask);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);
}
void loop()
{
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);

  if (pos)  // check if valid tcp data is received
  {
    char* data = (char *) Ethernet::buffer + pos;
    if (strstr(data, "GET /Setup"))
    {
      ether.httpServerReply(setupPage()); // send web page data
    }
    else
    { if (strstr(data, "POST"))
      {

        if (strstr(data, "led3=on")) {
          led = 1 ;
          digitalWrite(3, HIGH);
        }
        else {
          led = 0;  //Serial.println(ether.findKeyVal( data+1, led, 10, "led3"));
          digitalWrite(3, LOW);
        }

        //      Serial.println(data);
        Serial.println(led);
      }

      ether.httpServerReply(homePage()); // send web page data
    }
    Serial.println(data);

  }
}

static word setupPage() {
  bfill.emit_p(PSTR(
                 "$F"
                 "\r\n"
                 "<html>"
                 "<title>Server - первый</title>"
                 "<body>"
                 "<h2>Страничка настройки</h2>"
                 "</body>"
                 "</html>"
               ),
               okHeader);
  return bfill.position();
}

static word homePage() {
  long t = millis() / 1000;
  word h = t / 3600;
  byte m = (t / 60) % 60;
  byte s = t % 60;
  bfill = ether.tcpOffset();

  //ether.printIp("Server: ", ether.myip);

  bfill.emit_p(PSTR(
                 "$F"
                 "\r\n"
                 //    "<meta http-equiv='refresh' content='5'/>"
                 "<html>"
                 "<title>Server - первый</title>"
                 "<body>"
                 "<h1>Время работы $D$D:$D$D:$D$D</h1>"
                 "<a href='/Setup'>Регистрация</a> "
               ),
               okHeader,
               h / 10, h % 10, m / 10, m % 10, s / 10, s % 10);
  bfill.emit_p(PSTR(
                 "<form action='' method='POST'>"
                 "<p><b>LED control</b></p>"
                 //"<input type='radio' name='led' value='on'>Включить<Br>"
                 //"<input type='radio' name='led' value='off'>Выключить</p>"u
               ));

  bfill.emit_p(PSTR("<input type='checkbox' name='led3' onClick='submit()'"));

  if (digitalRead(3) == HIGH)
    bfill.emit_p(PSTR(" checked>Off<br>"));
  else
    bfill.emit_p(PSTR(">On<br>"));

  bfill.emit_p(PSTR(//"<p><input type='submit' value='Обновить'></p>"
                 "</form>"
                 "</body>"
                 "</html>"
               ));

  return bfill.position();
}


