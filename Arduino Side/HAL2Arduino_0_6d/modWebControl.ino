#define useWebControl false
#if useWebControl
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* ssid = "ThuisgroepOW";
const char* password = "sukkel67919772!";

#define TELEGRAM_BOT_TOKEN "5385827741:AAH8eyYhJUSEP5JS5nwYMO6rMr_mffNsmak"

WiFiClientSecure client;
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, client);


int delayBetweenChecks = 1000;
unsigned long lastTimeChecked;

String ontv_text;

int currentBedTemp = 0;
int currentHeadTemp = 0;
int TimeRemaining = 0;
int PrinterStatus = 0;
int ProgramStatus = 0;

String text = "";
String temp = "";
String old_text = "";
int numNewMessages;

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

  server.begin();  //Start the server
}

void WebControlLoop() {
  server.handleClient();
  loop_telegram(text, old_text, numNewMessages);
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
  server.send(200, "text/json", output);
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
  if (Status == 1) {
    Send_Message("Program running");
  } else {
    Send_Message("Program not running");
  }
}

void SetTimeRemaining(int SecondsRemaining) {
  TimeRemaining = SecondsRemaining;
}

//TELEGRAM METHODES

void Send_Message(String message) {
  bot.sendMessage("1946205342", message, "Markdown");
}

void handleNewMessages(int numNewMessages) {

  for (int i = 0; i < numNewMessages; i++) {
    if (bot.messages[i].chat_id.equals("1946205342")) {
      // If the type is a "callback_query", a inline keyboard button was pressed
      if (bot.messages[i].type == F("callback_query")) {
        ontv_text = bot.messages[i].text;

        if (ontv_text == F("Stop")) {
          Serial.println("510 0 0;");
          Serial.println("802 0 0;");
        } else if (ontv_text == F("Pause")) {
          Serial.println("509 0 0;");
        }
      } else {
        String chat_id = String(bot.messages[i].chat_id);
        String text = bot.messages[i].text;
        if (text == F("/options")) {

          // Keyboard Json is an array of arrays.
          // The size of the main array is how many row options the keyboard has
          // The size of the sub arrays is how many coloums that row has

          // "The Text" property is what shows up in the keyboard
          // The "callback_data" property is the text that gets sent when pressed

          String keyboardJson = F("[[{ \"text\" : \"Stop\", \"callback_data\" : \"Stop\" },{ \"text\" : \"Pause\", \"callback_data\" : \"Pause\" }]]");
          bot.sendMessageWithInlineKeyboard(chat_id, "Controls ", "", keyboardJson);
        }

        if (text == F("/Status")) {
          String message = "BedTemp: " + String(currentBedTemp) + " HeadTemp: " + String(currentHeadTemp) + " Time remaining: " + String(TimeRemaining);
          Send_Message(message);
        }

        // When a user first uses a bot they will send a "/start" command
        // So this is a good place to let the users know what commands are available
        if (text == F("/start")) {

          bot.sendMessage(chat_id, "/options : returns the inline keyboard\n /Status the temp and time remaining", "Markdown");
        }
      }
    }
  }
}

void loop_telegram(String& text, String old_text, int& numNewMessages) {
  if (millis() > lastTimeChecked + delayBetweenChecks) {

    // getUpdates returns 1 if there is a new message from Telegram
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);


    if (numNewMessages) {
      //  bot.sendMessage("1946205342", "eric  jj", "Markdown");
      //Send_Message("TESTING");
      handleNewMessages(numNewMessages);
      text = ontv_text;
    }

    lastTimeChecked = millis();
  }
}
#endif
