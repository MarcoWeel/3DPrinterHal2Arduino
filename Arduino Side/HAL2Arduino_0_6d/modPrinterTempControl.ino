#define useTemperature true
// This modFile controls 3dprinter temperature functions.
#if useTemperature
const long timeInterval = 500;
unsigned long timePreviousMillis = 0;
int countHead = 0;
int countBed = 0;

bool headLoopDone = false;
bool bedLoopDone = false;

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
bool IsStartupHead = true;
bool IsStartupBed = true;

int CalculateHeadTemp() {
  headLoopDone = false;
  int errorCount = 0;
  while (!headLoopDone) {
    float logR2, R2, T;
    int TcAverage = 0;
    Vo1 = analogRead(HeadAnalogPin);
    R2 = R1 * (1023.0 / (float)Vo1 - 1.0);
    logR2 = log(R2);
    T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
    Tc = T - 273.15;

    if (Tc < (TcHeadOld + 10) && Tc > (TcHeadOld - 10) || IsStartupHead && Tc > 0) {
      TcCombinedHead += Tc;
      countHead++;
      errorCount = 0;
    } else {
      errorCount++;
    }
    if (countHead == 10 || countHead > 10) {
      TcAverage = TcCombinedHead / countHead;
      if (TcAverage < (TcHeadOld + 20) && TcAverage > (TcHeadOld - 20) || IsStartupHead) {
        Serial.print("504 0 ");
        Serial.print(TcAverage);
        Serial.println(";");

        Serial.print("800 0 ");
        Serial.print(TcAverage);
        Serial.println(";");
        TcHeadOld = TcAverage;
        TcCombinedHead = 0;
        countHead = 0;
        IsStartupHead = false;
        headLoopDone = true;
        return TcAverage;
      }
    } else if (errorCount > 10) {
      Serial.println(errorCount);
      Serial.println("FAILHEAD");
      Serial.println("507 0 0;");
      break;
    }
  }
}

int CalculateBedTemp() {
  int errorCount = 0;
  bedLoopDone = false;
  while (!bedLoopDone) {
    float logR2, R2, T;
    int TcAverage;
    Vo2 = analogRead(BedAnalogPin);
    R2 = R1 * (1023.0 / (float)Vo2 - 1.0);
    logR2 = log(R2);
    T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
    Tc = T - 273.15;
    if (Tc < (TcBedOld + 20) && Tc > (TcBedOld - 20) || IsStartupBed && Tc > 0) {
      TcCombinedBed += Tc;
      countBed++;
      errorCount = 0;
    } else {
      errorCount++;
    }
    if (countBed == 10 || countBed > 10) {
      TcAverage = TcCombinedBed / 10;
      //.println(TcAverage);
      //Serial.println(TcBedOld);
      if (TcAverage != TcBedOld && TcAverage < (TcBedOld + 20) && TcAverage > (TcBedOld - 20) || IsStartupBed) {
        Serial.print("503 0 ");
        Serial.print(TcAverage);
        Serial.println(";");

        Serial.print("801 0 ");
        Serial.print(TcAverage);
        Serial.println(";");
        TcBedOld = TcAverage;
        TcCombinedBed = 0;
        countBed = 0;
        IsStartupBed = false;
        bedLoopDone = true;
        return TcAverage;
      }
    } else if (errorCount > 10) {
      Serial.println("FAILBED");
      Serial.println("508 0 0;");
      break;
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
  Serial.println("507 0 0;");
  HeadStatus = false;
  digitalWrite(HeadRelayPin, HIGH);
  HeadHeating = false;
  HeadTargetTemperature = HeadTargetTemp;
}

void SetBedTemperature(int BedTargetTemp) {
  Serial.println("508 0 0;");
  BedStatus = false;
  digitalWrite(BedRelayPin, LOW);
  BedHeating = false;
  BedTargetTemperature = BedTargetTemp;
}

void ControlRelays(int BedTemp, int HeadTemp) {
  HeadRelay(HeadTemp, HeadTargetTemperature);
  BedRelay(BedTemp, BedTargetTemperature);
}

void HeadRelay(int temp, int tempExpected) {
  if (HeadHeating) {
    if (tempExpected < temp - 1 && temp > -270 && tempExpected + 5 > temp) {
      digitalWrite(HeadRelayPin, HIGH);
      HeadHeating = false;
      if (HeadStatus == false) {
        HeadStatus = true;
      }
    } else if (temp < tempExpected - 20 && HeadStatus == true || temp > tempExpected + 20 && HeadStatus == true) {
      Serial.println("510 0 0;");  // Kan fout zijn;
      SetHeadTemperature(0);
      digitalWrite(HeadRelayPin, HIGH);
      HeadStatus = false;
    }
  } else {
    if (tempExpected - 1 > temp && temp > -270) {
      HeadHeating = true;
      digitalWrite(HeadRelayPin, LOW);
    }
  }
  if (temp > 45) {
    digitalWrite(FanRelayPin, LOW);
  } else {
    digitalWrite(FanRelayPin, HIGH);
  }
  if (HeadStatus == false && tempExpected - 1 < temp && tempExpected < temp + 5 && temp > -270 && tempExpected != 0) {
    HeadStatus = true;
    Serial.println("507 0 1 508;");
  }
}

void BedRelay(int temp, int tempExpected) {
  if (BedHeating) {
    if (tempExpected < temp - 1 && temp > -270 && tempExpected + 5 > temp) {
      digitalWrite(BedRelayPin, LOW);
      BedHeating = false;
      if (BedStatus == false) {
        Serial.println("508 0 1 509;");
        BedStatus = true;
      }
    } else if (temp < tempExpected - 20 && BedStatus == true || temp > tempExpected + 20 && BedStatus == true) {
      Serial.println("510 0 0;");  // Kan fout zijn;
      SetBedTemperature(0);
      digitalWrite(BedRelayPin, LOW);
      BedStatus = false;
    }
  } else {
    if (tempExpected - 1 > temp && temp > -270 || tempExpected > temp + 5 && temp > -270) {
      BedHeating = true;
      digitalWrite(BedRelayPin, HIGH);
    }
  }
  if (BedStatus == false && tempExpected < temp - 1 && tempExpected < temp + 5 && temp > -270 && tempExpected != 0) {
    BedStatus = true;
    Serial.println("508 0 1 509;");
  }
}


void temperatureLoop() {
  unsigned long currentMillis = millis();
  if (currentMillis - timePreviousMillis >= timeInterval) {
    timePreviousMillis = currentMillis;
    ControlRelays(CalculateBedTemp(), CalculateHeadTemp());
  }
}

#endif
