#define LED 0
#define BUTTON 1

#include <SoftwareSerial.h>

SoftwareSerial mySerial(3, 4); // RX, TX
int ledState = HIGH;
int buttonState = 0;
int lastButtonState = LOW;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
String message;

void setup() {
   mySerial.begin(9600);
   pinMode(LED, OUTPUT);
   pinMode(BUTTON, INPUT);
}

void button_update(){
  int reading = digitalRead(BUTTON);

  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
        mySerial.println("AT+BUTTON_1=pressed");
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}

void print_led_state() {
  mySerial.print("AT+LED=");
  mySerial.println(ledState);
}

void turn_led(int state) {
  ledState = state;
  print_led_state();
}

void toggle_led(){
  turn_led(!ledState);
}

void serial_update(){
  while (mySerial.available() > 0) {
    char recieved = mySerial.read();
    message += recieved;

    message.trim();
    // Process message when new line character is recieved
    if (recieved == '\n') {
      if(message == "AT+LED=1") {turn_led(HIGH);}
      if(message == "AT+LED=0") { turn_led(LOW); }
      if(message == "AT+LED=t") { toggle_led(); }
      if(message == "AT+LED=?") { print_led_state(); }

      message = "";
    }
  }
}

void loop() {
  // check button
  button_update();

  // set the LED:
  digitalWrite(LED, ledState);

  // listen serial
  serial_update();
}
