#define useWebControl false
#if useWebControl
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* ssid = "ThuisgroepOW";
const char* password =  "sukkel67919772!";

#define TELEGRAM_BOT_TOKEN "TOKEN"

WiFiClientSecure client;
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, client);


int delayBetweenChecks = 1000;
unsigned long lastTimeChecked;

String ontv_text;

int currentBedTemp;
int currentHeadTemp;
int TimeRemaining = 0;
int PrinterStatus = 0;
int ProgramStatus = 0;

WiFiClient client;

ESP8266WebServer server(8083);

void SetupWebControl() {
  WiFi.begin(ssid, password);  //Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED) {  //Wait for connection
    delay(500);
    Serial.println("Waiting to connect...");
  }
  client.setInsecure();
  Serial.println("Done");
  server.enableCORS(true);
  server.on("/stop", handleStop);
  server.on("/pause", handlePause);
  server.on("/temp", handleGet);
  server.on("/online", CheckIfOnline);

  server.begin(); //Start the server
}

void WebControlLoop() {
  server.handleClient();
}

void CheckIfOnline() {
  server.send(200);
}

void handleStop() {
  server.send(200);
  Serial.println("510 0 0;");
  Serial.println("802 0 0;");
}

void handlePause() {
  server.send(200);
  Serial.println("509 0 0;");
}

void handleGet() {
  DynamicJsonDocument doc(1024);
  doc["bedTemp"] = currentBedTemp;
  doc["headTemp"] = currentHeadTemp;
  doc["timeRemaining"] = TimeRemaining;
  doc["printerStatus"] = PrinterStatus;
  doc["programStatus"] = ProgramStatus;
  String output;
  serializeJson(doc, output);
  server.send(200, "text/json" , output );
}

void SetHeadTempInternal(int headTemp) {
  currentHeadTemp = headTemp;
}

void SetBedTempInternal(int bedTemp) {
  currentBedTemp = bedTemp;
}

void SetPrinterStatus(int Status) {
  PrinterStatus = Status;
}

void SetProgramStatus(int Status) {
  ProgramStatus = Status;
}

void SetTimeRemaining(int SecondsRemaining) {
  TimeRemaining = SecondsRemaining;
}

//TELEGRAM METHODES

void handleNewMessages(int numNewMessages) {

  for (int i = 0; i < numNewMessages; i++) {

    Serial.print("chat_id");Serial.println(bot.messages[i].chat_id);
    if (bot.messages[i].chat_id.equals("9952488")) {
      // If the type is a "callback_query", a inline keyboard button was pressed
      if (bot.messages[i].type ==  F("callback_query")) {
        ontv_text = bot.messages[i].text;
        Serial.print("Call back button pressed with text: ");
        Serial.println(ontv_text);

        if (ontv_text == F("ON")) {
        } else if (ontv_text == F("OFF")) {
        } else if (ontv_text.startsWith("TIME")) {
          ontv_text.replace("TIME", "");
          int timeRequested = ontv_text.toInt();

          lightTimerActive = true;
          lightTimerExpires = millis() + (timeRequested * 1000 * 60);
        }
      } else {
        String chat_id = String(bot.messages[i].chat_id);
        String text = bot.messages[i].text;

        if (ontv_text == F("/options")) {

          // Keyboard Json is an array of arrays.
          // The size of the main array is how many row options the keyboard has
          // The size of the sub arrays is how many coloums that row has

          // "The Text" property is what shows up in the keyboard
          // The "callback_data" property is the text that gets sent when pressed

          String keyboardJson = F("[[{ \"text\" : \"ON\", \"callback_data\" : \"ON\" },{ \"text\" : \"OFF\", \"callback_data\" : \"OFF\" }],");
          keyboardJson += F("[{ \"text\" : \"10 Mins\", \"callback_data\" : \"TIME10\" }, { \"text\" : \"20 Mins\", \"callback_data\" : \"TIME20\" }, { \"text\" : \"30 Mins\", \"callback_data\" : \"TIME30\" }]]");
          bot.sendMessageWithInlineKeyboard(chat_id, "Sadbhs Stars", "", keyboardJson);
        }

        // When a user first uses a bot they will send a "/start" command
        // So this is a good place to let the users know what commands are available
        if (ontv_text == F("/start")) {

          bot.sendMessage(chat_id, "/options : returns the inline keyboard\n", "Markdown");
        }
      }
    }
  }
}

void loop_telegram(String &text,String old_text,int &numNewMessages) {
  if (millis() > lastTimeChecked + delayBetweenChecks)  {

    // getUpdates returns 1 if there is a new message from Telegram
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    Serial.print("numNewM ");Serial.print(numNewMessages);Serial.print("bot.last ");
    Serial.println(bot.last_message_received);


    if (numNewMessages) {
      Serial.println("got response");
      bot.sendMessage("9952488", "eric  jj", "Markdown");
      handleNewMessages(numNewMessages);
      text = ontv_text;
    }

    lastTimeChecked = millis();

    if (lightTimerActive && millis() > lightTimerExpires) {
      lightTimerActive = false;
      digitalWrite(LED_PIN, LOW);
    }
  }
}
#endif
