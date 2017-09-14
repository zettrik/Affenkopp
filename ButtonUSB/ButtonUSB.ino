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

const int button1 = 0; // arduino mega pin: A0
const int led1 = 13; // built in LED

int button1_state = 0;
int button1_state_last = 0;

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(button1, INPUT);
  digitalWrite(led1, 1);
  Serial.begin(115200);
}


void loop() {
  delay(50);
  digitalWrite(led1, 1);
  button1_state = analogRead(button1);
  
  if ((button1_state - button1_state_last) <= -100) {
    Serial.println(1);
    Serial.flush();
    //Serial.println(button1_state);
    digitalWrite(led1, 0);
  }
  if ((button1_state - button1_state_last) >= 100) {
    Serial.println(0);
    Serial.flush();
    //Serial.println(button1_state);
    digitalWrite(led1, 0);
  }
  button1_state_last = button1_state;
}
