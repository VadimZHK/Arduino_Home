#include <EtherCard.h>
//#include <EEPROM.h>
#include <OneWire.h>


byte subnet[] = { 255, 255, 0, 0 };
byte mac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };
byte ip[] = { 192, 168, 127, 55 };
byte gateway[] = { 191, 168, 127, 232 };
byte dns[] = { 8, 8, 8, 8 };
int led;
char element[30] = ",[";
char x[5];
float fl;

const char okHeader[] PROGMEM =
  "HTTP/1.0 200 OK\r\n"
  "Content-Type: text/html; charset=utf-8\r\n"
  "Pragma: no-cache\r\n\r\n"
  ;
const char okError[] PROGMEM =
  "HTTP/1.1 404 Not Found\r\n\r\n"
  ;

/*const char http_Found[] PROGMEM =
  "HTTP/1.0 302 Found\r\n"
  "Location: /\r\n\r\n"
  ;*/
const char http_Button[] PROGMEM =
  // "<input type='submit' name='setup' value='setup'>"
  "<input type='submit' name='main' value='main'>"
  "<input type='submit' name='svg' value='svg'>"
  "<br>\r\n"
  ;
const char http_Header[] PROGMEM =
  "<html>"
  "<head>"
  "<link rel='icon' type='image/ico' href='https://www.arduino.cc/favicon.ico' type='image/x-icon'/>"
  "<title>Server - первый</title>"
  "</head>"
  "<body>"
  ;
const char http_End[] PROGMEM =
  "</form>"
  "</body>"
  "</html>"
  ;

//
//  имеем конфигурацию нашей супер-пупер программы
//
struct MyTemp {
  int tdom;
  int tdvor;
  int troof;
  int tbuild;
} my_temp;

MyTemp arrayTemp[8];
byte aLen;
//char element[30] = "[110,-44.2,-40.9,-40.4,-38.8]";

byte Ethernet::buffer[1200];

BufferFiller bfill;

//OneWire  ds(3);  // on pin 10 (a 4.7K resistor is necessary)
//double celsius;

byte vOffset;

void setup()
{
  Serial.begin(250000);
  //Serial.println(0 % 16);
  if (ether.begin(sizeof Ethernet::buffer, mac, 10) == 0)
    Serial.println(F("Failed to access Ethernet controller"));

  Serial.println(F("Ok\n"));
  // Serial.println(F(mac));
  //  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);//HIGH);//
  //  digitalWrite(8,1);
  //Ethernet.begin(mac,myIP);
  //ether.hisport = 443;
  ether.staticSetup(ip, gateway, dns, subnet);
  ether.printIp("My IP: ", ether.myip);
  ether.printIp("Netmask: ", ether.netmask);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);
 // Serial.println(sizeof(my_temp));
  //EEPROM.get(0, my_temp);

  //Serial.println(my_temp.tdom);
  //Serial.println(my_temp.tdvor);
  //Serial.println(my_temp.troof);
  //Serial.println(my_temp.tbuild);
  // my_temp.tdom =151;
  // my_temp.tdvor =155;
  //vData = String("");
  vOffset = 0;
  ///EEPROM.put(0, my_temp);
  //storeTemp();
  //readStore();
//  Serial.println("Size");
//  Serial.println(sizeof(arrayTemp)/sizeof(MyTemp));
  aLen =sizeof(arrayTemp)/sizeof(MyTemp);
}
void loop()
{
  long tt = millis() / 1000;
  // word h = t / 3600;
  // byte mm = (tt / 60) % 60;
  byte ss = tt % 60;

  if ((ss >= 0 and ss < 30 and vOffset % 2 == 0)
      or (ss >= 30 and vOffset % 2 != 0))
  {
   
    Serial.print(ss); Serial.print(" sek ");
    Serial.print(vOffset); Serial.println(" offset");
    storeTemp();
    vOffset ++;
    if (vOffset >= aLen) {
      vOffset = 0;
    }
  }

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
    /*else if (strstr(data, "setup=setup"))
      {
      Serial.println(data);
      ether.httpServerReply(setupPage());//homePage()); // send web page data
      }*/
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
                 "([['X','дом','улица','чердак','бытовка']"
                 //",[0, 8, 8, 8, 8],"
                 //  ",[22, 7, 8, 10, 20]"
               ), okHeader);
  //readStore();
  element[0] = ',';
  element[1] = '[';

  Serial.println(element);

  for (int i = 0; i < aLen; i++) {
    element[2] = 0;
    x[0] = 0;
    dtostrf(i, strlen(x)/*Полная_длина_строки*/, 0/*Количество_символов_после_запятой*/, x);
    strcat(element, x);
    strcat(element, ",");

    fl = arrayTemp[i].tdom / 10.0;
    dtostrf(fl, strlen(x)/*Полная_длина_строки*/, 1/*Количество_символов_после_запятой*/, x);
    strcat(element, x);
    strcat(element, ",");
    //  Serial.println(element);
    fl = arrayTemp[i].tdvor / 10.0;
    dtostrf(fl, strlen(x)/*Полная_длина_строки*/, 1/*Количество_символов_после_запятой*/, x);
    strcat(element, x);
    strcat(element, ",");
    //   Serial.println(element);
    fl = arrayTemp[i].troof / 10.0;
    dtostrf(fl, strlen(x)/*Полная_длина_строки*/, 1/*Количество_символов_после_запятой*/, x);
    strcat(element, x);
    strcat(element, ",");
    //   Serial.println(element);
    fl = arrayTemp[i].tbuild / 10.0;
    dtostrf(fl, strlen(x)/*Полная_длина_строки*/, 1/*Количество_символов_после_запятой*/, x);
    strcat(element, x);
    strcat(element, "]");
    Serial.println(element);
    //
    bfill.emit_p(PSTR("$S"), element);
  }

  //  const char *str2 = vData.c_str();
  //  Serial.println(vData);
  //  Serial.println(*str2);
  // bfill.emit_p(PSTR("$S"), str2);
  /*  "[0, 8, 8, 8, 8],"
    "[5, 8.5, 6.6, 5, 6],"
    "[7.5, -7, 5, 6.4, 7],"
    "[10, -5, 6, 7, 8],"
    "[15, 0, 7, 8.1, 9],"
    "[20, 3, 7.9, 7.1, 19],"
    "[24, 7, 8, 10, 20]"*/
  bfill.emit_p(PSTR(
                 "]);"
                 "var options = {"
                 "};"
                 "var chart=new google.visualization.LineChart(document.getElementById('chart_div'));"
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
               ), http_Button, http_End);
  // readStore();
  // Serial.println(vData);
  return bfill.position();
}
/*
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
  }*/
/*
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
*/
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
               okHeader
               , http_Header,
               h / 10, h % 10, m / 10, m % 10, s / 10, s % 10
               , http_Button
              );
  bfill.emit_p(PSTR(
                 "<b>LED control</b><br>"
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

void storeTemp() {
 // Serial.println(vOffset);
  arrayTemp[vOffset].tdom = analogRead(A0);
  arrayTemp[vOffset].tdvor = analogRead(A1);
  arrayTemp[vOffset].troof = analogRead(A2);
  arrayTemp[vOffset].tbuild = analogRead(A3);
  //EEPROM.put(vOffset, my_temp);
}

void readStore() {
  // MyTemp mt;
  //  vData = "";

  for (int i = 0 ; i < aLen; i++) {
    // EEPROM.get(i * 8, mt);
    // mt.tdom =  arrayTemp[i].tdom//;
    //   if (mt.tdom == -1) break;
    //  (i!=0)
    /* vData.concat(",[");
      vData.concat(i);
      vData.concat(",");
      vData.concat(arrayTemp[i].tdom / 10.0);
      vData.concat(",");
      vData.concat(arrayTemp[i].tdvor / 10.0);
      vData.concat(",");
      vData.concat(arrayTemp[i].troof / 10.0);
      vData.concat(",");
      vData.concat(arrayTemp[i].tbuild / 10.0);
      vData.concat("]");*/
  }

  //Serial.println(vData);
  return ;

}


