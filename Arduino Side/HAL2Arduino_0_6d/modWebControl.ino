#define useWebControl true
#if useWebControl
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "ThuisgroepOW";
const char* password =  "sukkel67919772!";

int currentBedTemp;
int currentHeadTemp;

WiFiClient client;

ESP8266WebServer server(8083);

void SetupWebControl() {
  WiFi.begin(ssid, password);  //Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED) {  //Wait for connection
    delay(500);
    Serial.println("Waiting to connect...");
  }
  server.enableCORS(true);
  server.on("/stop", handleStop);
  server.on("/pause", handlePause);
  server.on("/temp", handleTemp);
  
  server.begin(); //Start the server
}

void WebControlLoop(){
  server.handleClient();
}

void handleStop() { 
  server.send(200);
  Serial.println("510 0 0;");
}

void handlePause() { 
  server.send(200);
  Serial.println("509 0 0;");
}

void handleTemp(){
  DynamicJsonDocument doc(1024);
  JsonObject root = doc.as<JsonObject>();
  root["bedTemp"] = currentBedTemp;
  root["headTemp"] = currentHeadTemp;
  String output;
  serializeJson(doc, output);
  server.send(200, "text/json" , output );
}

void SetHeadTempInternal(int headTemp){
  currentHeadTemp = headTemp;
}

void SetBedTempInternal(int bedTemp){
  currentBedTemp = bedTemp;
}
#endif
