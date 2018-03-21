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
  "Pragma: no-cache\r\n\r\n"
  ;
const char okError[] PROGMEM =
  "HTTP/1.1 404 Not Found\r\n\r\n"
  //"Content-Type: text/html; charset=utf-8\r\n"
  //"Pragma: no-cache\r\n\r\n"
  ;

const char http_Found[] PROGMEM =
  "HTTP/1.0 302 Found\r\n"
  "Location: /\r\n\r\n"
  ;
const char http_Button[] PROGMEM =
  "<input type='submit' name='setup' value='setup'>"
  "<input type='submit' name='main' value='main'>"
  "<input type='submit' name='svg' value='svg'>"
  "<br>\r\n"
  ;
const char http_Header[] PROGMEM =
  "<html>"
  "<head>"
  "<link rel='icon' type='image/ico' href='https://www.arduino.cc/favicon.ico' type='image/x-icon' />"
  "<title>Server - первый</title>"
  "</head>"
  "<body>"
  ;
const char http_End[] PROGMEM =
  "</form>"
  "</body>"
  "</html>"
  ;

byte Ethernet::buffer[1200];

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

   // Serial.println(data);

    if (strstr(data, "GET /favicon.ico"))
    {
      // Serial.println("GET ///////////////");
    }
    else if (strstr(data, "GET / HTTP"))
    {
      ether.httpServerReply(homePage());
    }
    else if (strstr(data, "GET /"))
    {
      //     Serial.println(data);
      ether.httpServerReply(errorPage());//homePage()); // send web page data
    }
    else if (strstr(data, "svg=svg"))
    {
      Serial.println(data);
      ether.httpServerReply(chartPage());//homePage()); // send web page data
    }
    else if (strstr(data, "setup=setup"))
    {
      Serial.println(data);
      ether.httpServerReply(setupPage());//homePage()); // send web page data
    }
    else if (strstr(data, "main=main"))
    {
      Serial.println(data);
      ether.httpServerReply(homePage());//setupPage()); // send web page data
    }
    else

    { if (strstr(data, "POST /"))
      {
        if (strstr(data, "save=save")) {
          if (strstr(data, "led3=on")) {
            led = 1;
            digitalWrite(3, HIGH);
          }
          else {
            led = 0;
            digitalWrite(3, LOW);
            while ( digitalRead(3) != LOW)//без нагрузки долго гаснет
              delay(10);
            Serial.println("погас");
          }

        }

        //      Serial.println(data);
        Serial.println(led);
      }
      //   Serial.println(data);

      ether.httpServerReply(homePage()); // send web page data
    }

  }
}
static word chartPage() {
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
                 "$F"
                 "<html>"
                 "<head>"
                 "<link rel='icon' type='image/ico' href='https://www.arduino.cc/favicon.ico' type='image/x-icon' />"
                 "<meta charset='utf-8'>"
                 "<script type='text/javascript' src='https://www.gstatic.com/charts/loader.js'></script>"
                 "<script type='text/javascript'>"
                 "google.charts.load('current', {packages:['corechart']});"
                 "google.charts.setOnLoadCallback(drawChart);"
                 "function drawChart() {"
                 "var data = google.visualization.arrayToDataTable"
                 "([['X', 'дом', 'улица', 'чердак', 'бытовка'],"
                 "[20, 8, 8, 8, 8],"
                 "[25, 8.5, 4, 5, 6],"
                 "[35, -7, 5, 6, 7],"
                 "[45, 7, 6, 7, 8],"
                 "[55, 7, 7, 8, 9],"
                 "[70, 7, 8, 9, 20]"
                 "]);"
                 "var options = {"
                 "};"
                 "var chart = new google.visualization.LineChart(document.getElementById('chart_div'));"
                 "chart.draw(data, options);"
                 "}"
                 "</script>"
                 "</head>"
                 "<body>"
                 "<form action='' method='post'>"
                 "<h1>Страничка графика</h1>"
                 "$F"
                 "<div id='chart_div' style='width: 900px; height: 500px;'></div>"
                 "$F"
               ), okHeader, http_Button, http_End);
  return bfill.position();
}

static word svgPage() {
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
                 "$F"
                 "$F"
                 "<form action='' method='post'>"
                 "<h1>Страничка SVG</h1>"
                 "$F"
                 "<svg><line x1='0' y1='0' x2='100' y2='100' stroke-width='2' stroke='rgb(0,0,0)'/></svg>"
                 "$F"
               ),
               okHeader, http_Header, http_Button, http_End); //http_Found);//


  return bfill.position();
}
static word setupPage() {
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
                 "$F"
                 "$F"
                 "<form action='' method='post'>"
                 "<h1>Страничка настройки</h1>"
                 "$F"
                 "$F"
               ),
               okHeader, http_Header, http_Button, http_End); //http_Found);//


  return bfill.position();
}
static word errorPage() {
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR("$F"),
               okError);
  return bfill.position();
}

static word homePage() {
  long t = millis() / 1000;
  word h = t / 3600;
  byte m = (t / 60) % 60;
  byte s = t % 60;
  int an;
  bfill = ether.tcpOffset();

  //ether.printIp("Server: ", ether.myip);

  bfill.emit_p(PSTR(
                 "$F"
                 "$F"
                 "<h1>Время работы $D$D:$D$D:$D$D</h1>"
                 "<form action='' method='post'>"
                 "$F"
               ),
               okHeader, http_Header,
               h / 10, h % 10, m / 10, m % 10, s / 10, s % 10
               , http_Button
              );
  bfill.emit_p(PSTR(

                 "<p><b>LED control</b></p>"

               ));
  an = analogRead(A0);
  Serial.println(an);
  bfill.emit_p(PSTR("<input type='text' value ='$D,$D' size='5' disabled>"
                    "<input type='checkbox' name='led3' ")
               , an / 10, an % 10);

  if (digitalRead(3) == HIGH)
    bfill.emit_p(PSTR(" checked>Off<br>"));
  else
    bfill.emit_p(PSTR(">On<br>"));

  bfill.emit_p(PSTR("<p><input type='submit' name='save' value='save'></p>"
                    "$F"
                   ), http_End);

  return bfill.position();
}


