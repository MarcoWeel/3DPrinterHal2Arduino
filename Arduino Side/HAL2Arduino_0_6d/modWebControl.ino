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
  Serial.println("Done");
  server.enableCORS(true);
  server.on("/stop", handleStop);
  server.on("/pause", handlePause);
  server.on("/temp", handleGet);
  server.on("/online", CheckIfOnline);
  
  server.begin(); //Start the server
}

void WebControlLoop(){
  server.handleClient();
}

void CheckIfOnline(){
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

void handleGet(){
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

void SetHeadTempInternal(int headTemp){
  currentHeadTemp = headTemp;
}

void SetBedTempInternal(int bedTemp){
  currentBedTemp = bedTemp;
}

void SetPrinterStatus(int Status){
  PrinterStatus = Status;
}

void SetProgramStatus(int Status){
  ProgramStatus = Status;
}

void SetTimeRemaining(int SecondsRemaining){
  TimeRemaining = SecondsRemaining;
}
#endif
