#define useMiscControl true
// This modFile controls misc 3dprinter functions.
#if useMiscControl
#include <Servo.h>
Servo myservo;  // create servo object to control a servo
int Touch = 0;
//const int ledPin = 13;       // the pin that the LED is attached to
const int servoPin = 2;
const byte interruptPin = 3;  // White probe output wire to Digital pin 2
const int SideFanRelayPin = 7;
const int filamentDetectorPin = 5;  
const long sensorTimeInterval = 250;
unsigned long sensorTimePreviousMillis = 0;

//PINS CHECKEN EN EVENTUEEL VERANDEREN

void miscPrinterControlSetup() {
  digitalWrite(SideFanRelayPin, HIGH);
  pinMode(SideFanRelayPin, OUTPUT);
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object
  //pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(filamentDetectorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), SendProbeSignal, RISING);  // off to on will interrupt & go to blink function MISS NAAR FALLING IPV RISING
}

void SideFanRelay(bool setting) {
  if (setting) {
    digitalWrite(SideFanRelayPin, LOW);
  } else {
    digitalWrite(SideFanRelayPin, HIGH);
  }
}

void StartProbing() {
  myservo.write(10);
}

void SendProbeSignal() {
  Serial.println("514 0 1;");
  myservo.write(90);
}

//LATER MISSCHIEN VERANDEREN NAAR ANDERE CODE VOOR VERPLAATSING KOP
void handlePausefromSensor() {
  Serial.println("509 0 0;");
}

bool filamentStatus = false;
long lastDebounceTime = 0;
long debounceDelay = 50;
//STILL ADD DEBOUNCING AND ONLY FIRING ONCE
void MiscLoop() {
  unsigned long currentMillis = millis();
  if (currentMillis - sensorTimePreviousMillis >= sensorTimeInterval) {
    sensorTimePreviousMillis = currentMillis;
    filamentStatus = digitalRead(filamentDetectorPin);
    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (filamentStatus) {
        handlePausefromSensor();
        lastDebounceTime = millis();  //set the current time
      } else {
        lastDebounceTime = millis();
      }
    }
  }
}
#endif
