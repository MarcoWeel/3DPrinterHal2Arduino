#define useTemperature false
// This modFile controls 3dprinter temperature functions.
#if useTemperature
const long interval = 100;
unsigned long previousMillis = 0;
int count = 0;

const int BedAnalogPin = A0;
const int HeadAnalogPin = A1;

int Vo1;
int Vo2;
float R1 = 4700;
float Tc;
float c1 = 0.8095472909e-3, c2 = 2.116627405e-4, c3 = 0.7058258450e-7;
int TcHeadOld = 0;
int TcBedOld = 0;
int TcCombinedHead = 0;
int TcCombinedBed = 0;

int CalculateHeadTemp() {
  float logR2, R2, T;
  int TcAverage = 0;
  Vo1 = analogRead(HeadAnalogPin);
  R2 = R1 * (1023.0 / (float)Vo1 - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tc = T - 273.15;
  if(Tc !> (TcHeadOld + 20) && Tc !< (TcHeadOld - 20)){
  TcCombinedHead += Tc;
  }
  if (count == 10) {
    TcAverage = TcCombinedHead / 10;
    if (TcAverage != TcHeadOld && TcAverage !< (TcHeadOld + 20) && TcAverage !> (TcHeadOld - 20)) {
      Serial.print("504 0");
      Serial.print(TcAverage);
      Serial.println(";");

      Serial.print("800 0");
      Serial.print(TcAverage);
      Serial.println(";");
      TcHeadOld = TcAverage;
    }
  }
}

int CalculateBedTemp() {
  float logR2, R2, T;
  int TcAverage;
  Vo2 = analogRead(BedAnalogPin);
  R2 = R1 * (1023.0 / (float)Vo2 - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tc = T - 273.15;
  if(Tc !> (TcBedOld + 20) && Tc !< (TcBedOld - 20)){
  TcCombinedHead += Tc;
  }
  if (count == 10) {
    TcAverage = TcCombinedBed / 10;
    if (TcAverage != TcBedOld && TcAverage !< (TcBedOld + 20) && TcAverage !> (TcBedOld - 20)) {
      Serial.print("503 0");
      Serial.print(TcAverage);
      Serial.println(";");

      Serial.print("801 0");
      Serial.print(TcAverage);
      Serial.println(";");
      TcBedOld = TcAverage;
    }
  }
}

const int BedRelayPin = 13;
const int HeadRelayPin = 8;
const int FanRelayPin = 9;
bool HeadHeating = false;
bool BedHeating = false;
int BedTargetTemperature = 0;
int HeadTargetTemperature = 0;
bool HeadStatus = false;
bool BedStatus = false;

void SetupRelayPins() {
  digitalWrite(BedRelayPin, LOW);
  digitalWrite(HeadRelayPin, HIGH);
  digitalWrite(FanRelayPin, HIGH);
  pinMode(BedRelayPin, OUTPUT);
  pinMode(HeadRelayPin, OUTPUT);
  pinMode(FanRelayPin, OUTPUT);
}

void SetHeadTemperature(int HeadTargetTemp) {
  Serial.println("506 0 0;");
  HeadStatus = false;
  HeadTargetTemperature = HeadTargetTemp;
}

void SetBedTemperature(int BedTargetTemp) {
  Serial.println("507 0 0;");
  BedStatus = false;
  BedTargetTemperature = BedTargetTemp;
}

void ControlRelays(int BedTemp, int HeadTemp) {
  HeadRelay(HeadTemp, HeadTargetTemperature);
  BedRelay(BedTemp, BedTargetTemperature);
}

void HeadRelay(int temp, int tempExpected) {
  if (HeadHeating) {
    if (tempExpected < temp - 1 && temp < -270) {
      digitalWrite(HeadRelayPin, HIGH);
      HeadHeating = false;
      if (HeadStatus == false) {
        HeadStatus = true;
      }
    }
    else if (temp < tempExpected - 30 && HeadStatus == true) {
      Serial.println("121 0 0;"); // Kan fout zijn;
      SetHeadTemperature(0);
      BedStatus == false;
    }
  }
  else {
    if (tempExpected - 1 > temp && temp < -270 || temp > tempExpected + 30 && BedStatus == true) {
      HeadHeating = true;
      digitalWrite(HeadRelayPin, LOW);
    }
  }
  if (temp > 45) {
    digitalWrite(FanRelayPin, LOW);
  }
  else {
    digitalWrite(FanRelayPin, HIGH);
  }
  if (HeadStatus == false && tempExpected < temp - 1 && tempExpected < temp + 5 && temp < -270) {
    HeadStatus = true;
    Serial.println("506 0 1 507;");
  }
}

void BedRelay(int temp, int tempExpected) {
  if (BedHeating) {
    if (tempExpected < temp - 1 && temp < -270) {
      digitalWrite(BedRelayPin, LOW);
      BedHeating = false;
      if (BedStatus == false) {
        Serial.println("507 0 1 508;");
        BedStatus = true;
      }
    }
    else if (temp < tempExpected - 30 && BedStatus == true || temp > tempExpected + 30 && BedStatus == true) {
      Serial.println("121 0 0;"); // Kan fout zijn;
      SetBedTemperature(0);
      BedStatus == false;
    }
  }
  else {
    if (tempExpected - 1 > temp && temp > -270 || tempExpected > temp + 5 && temp > -270) {
      BedHeating = true;
      digitalWrite(BedRelayPin, HIGH);
    }
  }
  if (BedStatus == false && tempExpected < temp - 1 && tempExpected < temp + 5 && temp > -270) {
    BedStatus = true;
    Serial.println("506 0 1 507;");
  }
}


void temperatureLoop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ControlRelays(CalculateBedTemp(), CalculateHeadTemp());
    count++;
  }
}

#endif
