#define useMiscControl false
// This modFile controls misc 3dprinter functions.
#if useMiscControl
const int SideFanRelayPin = 7;

void miscPrinterControlSetup() {
  digitalWrite(SideFanRelayPin, HIGH);
  pinMode(SideFanRelayPin, OUTPUT);
}

void SideFanRelay(bool setting) {
  if (setting) {
    digitalWrite(SideFanRelayPin, LOW);
  }
  else {
    digitalWrite(SideFanRelayPin, HIGH);
  }
}


#endif
