#include <WiFi.h>
#include <WiFiClientSecure.h>
WiFiClientSecure client;
 
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
 
//#include <string>
#include <HTTPClient.h>
#include "time.h"
 
#include <ArduinoJson.h>
 
#define ESP32_TITRE "ESP32-GOOGLE-CALENDAR"
 
// Set the number of entries to show - default is 10 - note that spacing between entries can push some out of the viewable area
const int calEntryCount = 10;
 
// Get calendar
char calendarServer[] = "script.google.com";
 
// Write the path for your google script to fetch calendar events
String calendarRequest = "https://script.google.com/macros/s/G_SCRIPT_ID/exec";
 
// Right now the calendarentries are limited to time and title
struct calendarEntries
{
  String calTime;
  String calTitle;
};
 
/////////////////////////////////////////////////////////
//                      FUNCTIONS                      //
/////////////////////////////////////////////////////////
bool displayCalendar();
String getURL(String url);
bool getRequest(char *urlServer, String urlRequest);
/////////////////////////////////////////////////////////
 
// setup
void setup() {
  Serial.begin(115200);
  Serial.println(F("\nESP32 GOOGLE CALENDAR TEST"));
 
  WiFiManager wifiManager;
  wifiManager.autoConnect(ESP32_TITRE);
 
  displayCalendar();
}
 
// loop
void loop() {
  //...
}
 
// Main display code - assumes that the display has been initialized
bool displayCalendar() {
  // Getting calendar from your published google script
  Serial.println(F("Getting calendar"));
  getRequest(calendarServer, calendarRequest);
 
  String outputStr = client.readString();
 
  // If redirected - then follow redirect - google always redirect to a temporary URL by default. Note that this at times fail resulting in an empty display. Consider detecting this and retrying.
  if (outputStr.indexOf("Location:") > 0) {
    String newURL = getURL(outputStr);
    //Serial.println("new url: " + newURL);
 
    getRequest(calendarServer, newURL);
 
    outputStr = client.readString();
  }
 
  int indexFrom = 0;
  int indexTo   = 0;
  int cutTo     = 0;
 
  String strBuffer = "";
 
  int count = 0;
  int line  = 0;
 
  indexFrom = outputStr.lastIndexOf("\n") + 1;
 
  struct calendarEntries calEnt[calEntryCount];
 
  // Fill calendarEntries with entries from the get-request
  while (indexTo >= 0 && line < calEntryCount) {
    count++;
    indexTo = outputStr.indexOf(";", indexFrom);
    cutTo = indexTo;
 
    if (indexTo != -1) {
      strBuffer = outputStr.substring(indexFrom, cutTo);
 
      indexFrom = indexTo + 1;
 
      //Serial.println(strBuffer);
 
      if (count == 1) {
        // Set entry time
        // Exclude end date and time to avoid clutter
        // Format is "Wed Feb 10 2020 10:00"
        calEnt[line].calTime = strBuffer.substring(0, 21);
      } else if (count == 2) {
        // Set entry title
        calEnt[line].calTitle = strBuffer;
      } else {
        count = 0;
        line++;
      }
    }
  }
 
  // Print calendar entries from first [0] to the last fetched [line-1] - in case there is fewer events than the maximum allowed
  for (int i = 0;  i < line; i++) {
    // Print event time
    Serial.println(calEnt[i].calTime);
 
    // Print event title
    Serial.print(calEnt[i].calTitle);
  }
 
  client.stop();
  return true;
}
 
// Generic code for getting requests - doing it a bit pure,
// as most libraries I tried could not handle the redirects from google
bool getRequest(char *urlServer, String urlRequest) {
  client.stop(); // close connection before sending a new request
 
  if (client.connect(urlServer, 443)) { // if the connection succeeds
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("GET " + urlRequest); // " HTTP/1.1"
    Serial.println("GET " + urlRequest);
    client.println("User-Agent: ESP32 LOLIN32 Receiver/1.1");
    client.println();
 
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 10000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        Serial.println("Connection timeout");
        return false;
      }
    }
  } else {
    Serial.println(F("Calendar connection did not succeed"));
  }
  return true;
}
 
/* Extract URL from http redirect - used especially for google as they always redirect to a new temporary URL */
String getURL(String url) {
  String tagStr = "Location: ";
  String endStr = "\n";
 
  int indexFrom = url.indexOf(tagStr) + tagStr.length();
  int indexTo   = url.indexOf(endStr, indexFrom);
 
  return url.substring(indexFrom, indexTo);
}