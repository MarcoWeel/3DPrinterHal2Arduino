// That's it your done. :)
// This is the program that will actually run things.
// There should be no need to modify it.
//Highest 500 command is 513
const long interval = 100;
unsigned long previousMillis = 0;
bool GotCommando = false;

void setup()
{
  //  pinMode(2, OUTPUT);
  //    while (!Serial) {
  //      digitalWrite(2, !digitalRead(2));
  //      delay(400);
  //    }
  Serial.begin(BAUD);
#if useEncoder
  encoderSetup();
#endif
#if useEncoderServo
  encoderServoSetup();
#endif
#if useTemperature
  SetupRelayPins();
#endif
#if useMiscControl
  miscPrinterControlSetup();
#endif
  Serial.setTimeout(statementTimeout);
  Serial.println("ok");
  for (int i = 0; i < 9; i++) {
    targetPos[i] = 0;
    targetPosOld[i] = 0;
#if autoSpeedAdjust
    deltaOld[i] = 1;
#endif
  }
}


long checksumOld = 0;
boolean debug = false;

void processCommand(long command, long dataOne, long dataTwo)
{
  if (command + dataOne + dataTwo != checksumOld) {
    if (command > 99 && command < 200) {
      // This is a command from the "motion" catagory.
#if useSpindle
      if (command == 130) { // motion.spindle-brake
        spindleBrake = dataTwo;
      }
      if (command == 131) { // motion.spindle-forward
        spindleSetDir = dataTwo;
      }
#if useEncoder
      if (command == 132) { // motion.spindle-index-enable
        sendSpindleIndexSignal = dataTwo;
        //          spindleIndexEnabled[dataOne]=dataTwo;
      }
#endif
      if (command == 133) { // motion.spindle-on
        spindleEnabled = dataTwo;
      }
      if (command == 134) { // motion.spindle-reverse
        spindleSetDir = !dataTwo;
      }
      if (command == 137) { // motion.spindle-speed-out
        spindleSetRpm = dataTwo;
      }
      if (command == 138) { // motion.spindle-speed-out-rps
        spindleSetRpm = dataTwo * 60;
      }
#endif
    } else if (command > 199 && command < 300) {
      // This is a command from the "axis" catagory.
    } else if (command > 299 && command < 400) {
      // This is a command from the "iocontrol" catagory.
    } else if (command > 300 && command < 600) {
      // This is a command from the "iocontrol" catagory.
#if useMiscControl
      if (command == 505 && dataTwo == 1) {
        SideFanRelay(true);
      }
      else if (command == 505 && dataTwo == 0) {
        SideFanRelay(false);
      }
#endif
#if useTemperature
      else if (command == 501) {
        SetBedTemperature(dataTwo / 10000);
      }
      else if (command == 502) {
        SetHeadTemperature(dataTwo / 10000);
      }
#endif
      if (command == 511) {
        SetPrinterStatus(dataTwo);
      }
      else if (command == 512) {
        SetProgramStatus(dataTwo);
      }
      else if (command == 513) {
        SetTimeRemaining(dataTwo);
      }
    } else if (command > 799 && command < 900) {
#if useWebControl
      if (command == 800) {
        SetHeadTempInternal(dataTwo);
      }
      else if (command == 801) {
        SetBedTempInternal(dataTwo);
      }
#if useTemperature
      else if (command == 802) {
        SetHeadTemperature(0);
        SetBedTemperature(0);
      }
#endif
#endif
    } else if (command > 989 && command < 999) {
      // This is a firmware query.
      GotCommando = true;
      if (command == 990) { // Firmware title.
        Serial.println(firmwareTitle);
      } else if (command == 991) { // Version info.
        Serial.println(Version);
      } else if (command == 992) { // unitId if used in a swarm.
        Serial.println(unitNumber);
      } else if (command == 993) { // request HAL hooks per command list.
        Serial.println(commandsRequested);
      } else if (command == 994) { // request HAL hooks per axis list.
        Serial.println(axisRequested);
      } else if (command == 995) { // toggle debugging output.
        debug = !debug;
        if (debug) {
          Serial.println("Debug: on");
        } else {
          Serial.println("Debug: off");
        }
      } else if (command == 996) { // Got 'green light' from host to begin normal operations.
#if useWebControl
        SetupWebControl();
#endif
        clientOps = true;
      }
    }
    // Anti-spam check.
    checksumOld = command + dataOne + dataTwo; // Should not have any, but just in case.
  }
}

void doUrgentStuff()
{
  /* This is a good place to call a functions to handle
     limit switches and NON-blocking
     stepper or servo/encoder controller routines;
     things that are time sensitive but also don't require
     everything else wait for it to complete.
  */
#if useEncoder
  readEncoders();
#endif
#if useEncoderServo
  moveEncoderServos();
#endif
}

void doIdleStuff()
{
  /* This is a good place to call less time sensitive
     functions like coolant/lube pumps, jog wheels,
     button controls, status LEDs and LCD updates.
  */
#if useTemperature
  temperatureLoop();
#endif
#if useWebControl
  WebControlLoop();
#endif
}

#define numWords 3
long integerValue[numWords];
boolean serRst = true;
int n = 0;
char incomingByte;
boolean makeNeg = false;

void validateCommand() {
  boolean commandValidated = false;
#if useDueNativePort
  if (debug) {
    SerialUSB.print("got: ");
    SerialUSB.print(integerValue[0]);
    SerialUSB.print(", ");
    SerialUSB.print(integerValue[1]);
    SerialUSB.print(", ");
    SerialUSB.print(integerValue[2]);
    SerialUSB.print(", ");
    SerialUSB.println(integerValue[3]);
  }
#else
  if (debug) {
    Serial.print("got: ");
    Serial.print(integerValue[0]);
    Serial.print(", ");
    Serial.print(integerValue[1]);
    Serial.print(", ");
    Serial.print(integerValue[2]);
    Serial.print(", ");
    Serial.println(integerValue[3]);
  }
#endif
  processCommand(integerValue[0], integerValue[1], integerValue[2]);
  commandValidated = true;
}

void sendWaitPacket(unsigned int val) {
  Serial.print("998 0 ");
  Serial.print(val);
  Serial.println(";");
}

void loop() {
  if (Serial.available()) {
    if (Serial.available() > 30) {
      sendWaitPacket(Serial.available());
    }
    if (serRst) {
      for (int i = 0; i < numWords; i++) {
        if (clientOps) {
          doUrgentStuff();
        } integerValue[i] = 0;
      };
      n = 0;
      serRst = false;
    }
    incomingByte = Serial.read();
    if (incomingByte == ' ') {
      if (makeNeg) {
        integerValue[n] *= -1;
        makeNeg = false;
      } n++;
      incomingByte = Serial.read();
    }
    if (incomingByte == ';') {
      if (makeNeg) {
        integerValue[n] *= -1;
        makeNeg = false;
      } validateCommand();
      serRst = true;
    }
    if (incomingByte == '\r') {
      serRst = true;
    }
    if (incomingByte == '\n') {
      serRst = true;
    }
    if (incomingByte == '-') { // got a "-" symbol.
      makeNeg = true; // make value negative.
    } else {
      if (incomingByte > 47 && incomingByte < 58) {
        integerValue[n] *= 10;  // shift left 1 decimal place
        if (!serRst)integerValue[n] = ((incomingByte - 48) + integerValue[n]);
      }
    }
  }
  if (!GotCommando) {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      Serial.println("ok");
    }
  }
  if (clientOps) {
    doUrgentStuff();
    doIdleStuff();
  }
}
