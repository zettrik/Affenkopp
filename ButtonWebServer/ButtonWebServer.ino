/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "your wifi ssid";
const char *password = "your wifi pw";
const int button1 = 5; // nodemcu pin: D1
const int button2 = 4; // nodemcu pin: D2
const int led1 = 16; // built in LED and D0
const int led2 =  0; // nodemcu pin: D4
const int analogInPin = A0;

int led1_state = 0;
int led2_state = 0;
int button1_state = 0;
int button2_state = 0;
int analog = 0;

IPAddress local_ip(127,0,0,0); // will be set via dhcp
ESP8266WebServer server(80);

void handleRoot() {
  digitalWrite(led1, 0);
  button1_state = digitalRead(button1);
  button2_state = digitalRead(button2);
  led1_state = digitalRead(led1);
  led2_state = digitalRead(led2);
  char temp[700];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  Serial.print(local_ip);
  Serial.print(" / ");
  Serial.println(millis());
  //Serial.println(system_get_time());

  snprintf(temp, 700,
"<html>\
<head>\
  <meta http-equiv='refresh' content='5'/>\
  <title>Affenkopp Augenpieker</title>\
  <style>\
    body { background-color: #000000; font-family: Arial, Helvetica, Sans-Serif; Color: #FF8C00; }\
  </style>\
</head>\
<body>\
  <h1>Huhu friends of the Telleraugen Apes!</h1>\
  <p>Uptime: %02d:%02d:%02d</p>\
  <p>Button 1: %01d</p>\
  <p>Button 2: %01d</p>\
  <p><a href=\"/buttons\">raw button infos</a></p>\
  <p><a href=\"/about\">about</a></p>\
</body>\
</html>",
    hr, min % 60, sec % 60, button1_state, button2_state );

  server.send(200, "text/html", temp);
  delay(1);
  digitalWrite(led1, 1);
}

void handleButtons() {
  char temp[100];
  // todo: recognize short press
  button1_state = digitalRead(button1);
  button2_state = digitalRead(button2);
  led1_state = digitalRead(led1);
  led2_state = digitalRead(led2);
  analog = analogRead(analogInPin);
  Serial.print(button1_state);
  Serial.print(",");
  Serial.print(button2_state);
  Serial.print(",");
  Serial.print(led1_state);
  Serial.print(",");
  Serial.print(led2_state);
  Serial.print(",");
  Serial.print(analog);
  Serial.print(",");
  //Serial.println(system_get_time());
  Serial.println(millis());

  digitalWrite(led1, 0 );
  //digitalWrite(led1, button1_state);
  digitalWrite(led2, button2_state);

  snprintf(temp, 100,
    "%01d,%01d,%01d,%01d,%04d,%d\n",
    button1_state, button2_state, led1_state, led2_state, analog, millis());
 
  server.send(200, "text/html", temp);
  delay(1);
  digitalWrite(led1, 1);
}

void handleNotFound() {
  digitalWrite(led1, 0);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  delay(1);
  digitalWrite(led1, 1);
}

void setup_webserver(){  
  // define html paths
  server.on("/", handleRoot);
  server.on("/about", []() {
    server.send(200, "text/plain", "Affenkopp mit Knopp - siehmaleineran.de");
    } );
  server.on("/buttons", handleButtons);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void setup_wifi(){
  WiFi.begin(ssid, password);
  Serial.println("waiting for wlan connection");
  while(WiFi.status() != WL_CONNECTED) {
    digitalWrite(led1, 0);
    delay(250);
    digitalWrite(led1, 1);
    delay(250);    
    digitalWrite(led1, 0);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  local_ip = WiFi.localIP();
  Serial.println(local_ip);
}

void setup() {
  delay(10);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  digitalWrite(led1, 1);
  digitalWrite(led2, 1);
  Serial.begin(115200);
  setup_wifi();
  setup_webserver();
}

void loop(void) {
  server.handleClient();
}
