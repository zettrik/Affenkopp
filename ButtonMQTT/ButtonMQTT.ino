/*
 * blinking onboard LED means:
 *   fast  - connecting to wireless network
 *   slow  - connecting to mqtt broker
 *   flash - sending bytes via mqtt
 *   on    - critical power supply
 * 
 * mqtt channels in use:
 *   buttons - message with state imediatly after a change
 *   node    - status messages
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "b0r7";
const char* password = "";
const char* mqtt_server = "192.168.1.1";
const int mqtt_port = 1883;
const int button1 = 5; // nodemcu pin: D1
const int button2 = 4; // nodemcu pin: D2
const int led1 = 16; // built in LED and D0
const int led2 =  0; // nodemcu pin: D4

int led1_state = 0;
int led2_state = 0;
int button1_state = 0;
int button2_state = 0;
int button1_state_last = 0;
int button2_state_last = 0;
int analog_in = 0;
unsigned long now = 0;
unsigned long before = 0;
char mqtt_message[60];

IPAddress local_ip(127,0,0,0); // will be set via dhcp

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  digitalWrite(led1, 1);
  digitalWrite(led2, 1);
  Serial.begin(115200);
  setup_wifi();
  setup_mqtt();
}

void setup_mqtt(){
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi(){
  WiFi.begin(ssid, password);
  Serial.println("waiting for wlan connection");
  while(WiFi.status() != WL_CONNECTED) {
    // blink slowly while connecting
    digitalWrite(led1, 0);
    delay(125);
    digitalWrite(led1, 1);
    delay(125);
    digitalWrite(led1, 0);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  local_ip = WiFi.localIP();
  Serial.println(local_ip);
  for (int i = 0; i < 10; i++) {
    // blink fast when successful
    digitalWrite(led1, 0);
    delay(25);
    digitalWrite(led1, 1);
    delay(25);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void mqtt_pub(char* channel, char* mqtt_message) {
    Serial.println(mqtt_message);
    client.publish(channel, mqtt_message);
    digitalWrite(led1, 0);
    delay(1);
    digitalWrite(led1, 1);
    // wait a moment to ignore button bouncing & prevent fast polling
    delay(10);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Affenkopp")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("node", "Affenkopp says hello!");        
      // ... and resubscribe to inbound messages
      client.subscribe("node_in");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");
      // blink'n wait 3 seconds before retrying
      digitalWrite(led1, 0);
      delay(1500);
      digitalWrite(led1, 1);
      delay(1500);
    }
  }
}

void check_battery() {
  /* these values were measured with 330kOhm:22kOhm voltage divider at A0
   * raw input  :: voltage divider :: ADC analog_in
   *  12V   ::  330mV   ::  101
   *   6V   ::  178mV   ::   53
   * 4.6V   ::  130mV   ::   43
   * 
   * todo: check
   *    high 3x 4.2V 12.6V
   *    low 3x 2,8V 8,4V
  */
  analog_in = analogRead(A0);
  // high power is very critical keep warning about until voltage drops
  while (analog_in >= 101) {
      Serial.println("Input power is too high!");
      snprintf(mqtt_message, 75, "Input power is too high! %04d, %ld", analog_in, now);
      mqtt_pub("node", mqtt_message);
    digitalWrite(led1, 0);
    delay(2000);
    digitalWrite(led1, 1);
    analog_in = analogRead(A0);
  }
  // warn about low power and continue after a second
  // very low voltage probably means we are running by usb
  if (analog_in <= 75 && analog_in >= 50) {
    Serial.println("Input power is getting low!");
    snprintf(mqtt_message, 75, "Input power is getting low! %04d, %ld", analog_in, now);
    mqtt_pub("node", mqtt_message);
    digitalWrite(led1, 0);
    delay(1000);
    digitalWrite(led1, 1);
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  button1_state = digitalRead(button1);
  button2_state = digitalRead(button2);
  now = millis();

  // poll still alive messages
  // millis() will start again from 0 about every 50 days
  /*
  if ((unsigned long)(now - before) >= 10000) {
    before = now;
    mqtt_pub("node", "Affenkopp still alive :)");
    snprintf(mqtt_message, 75, "%01d,%01d,%04d,%ld", button1_state, button2_state, analog_in, now);
    //mqtt_pub("buttons", mqtt_message);
  }
  */
  if (button1_state != button1_state_last) {
    button1_state_last = button1_state;
    snprintf(mqtt_message, 75, "%01d,%01d,%04d,%ld", button1_state, button2_state, analog_in, now);
    mqtt_pub("buttons", mqtt_message);
  }
  else if (button2_state != button2_state_last) {
    button2_state_last = button2_state;
    snprintf(mqtt_message, 75, "%01d,%01d,%04d,%ld", button1_state, button2_state, analog_in, now);
    mqtt_pub("buttons", mqtt_message);
  }

  check_battery();
}
