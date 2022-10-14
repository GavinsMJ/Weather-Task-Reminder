#include <WiFi.h>
#include <HTTPClient.h>
WiFiClientSecure client;
 
///////////////////////////////////////////
const char* ssid     = "ssid";
const char* password = "password";
///////////////////////////////////////////
 
 
const String GOOGLE_SCRIPT_ID       = "GOOGLE_SCRIPT_ID"; 
 
const int sendInterval = 996 * 5; // in millis, 996 instead of 1000 is adjustment, with 1000 it jumps ahead a minute every 3-4 hours
 
//-------------
 

// updated 04.12.2019
const char * root_ca = \
                       "-----BEGIN CERTIFICATE-----\n" \
                       "MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n" \
                       "A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n" \
                       "Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n" \
                       "MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n" \
                       "A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n" \
                       "hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n" \
                       "v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n" \
                       "eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n" \
                       "tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n" \
                       "C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n" \
                       "zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n" \
                       "mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n" \
                       "V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n" \
                       "bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n" \
                       "3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n" \
                       "J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n" \
                       "291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n" \
                       "ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n" \
                       "AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n" \
                       "TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n" \
                       "-----END CERTIFICATE-----\n";
 
String urldecode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == '+') {
      encodedString += ' ';
    } else if (c == '%') {
      i++;
      code0 = str.charAt(i);
      i++;
      code1 = str.charAt(i);
      c = (h2int(code0) << 4) | h2int(code1);
      encodedString += c;
    } else {
 
      encodedString += c;
    }
 
    yield();
  }
 
  return encodedString;
}
 
String urlencode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
      //encodedString+=code2;
    }
    yield();
  }
  return encodedString;
}
 
unsigned char h2int(char c) {
  if (c >= '0' && c <= '9') {
    return ((unsigned char)c - '0');
  }
  if (c >= 'a' && c <= 'f') {
    return ((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <= 'F') {
    return ((unsigned char)c - 'A' + 10);
  }
  return (0);
}
 
// setup
void setup() {
  Serial.begin(115200);
  delay(10);
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  Serial.println("Started");
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
 
  Serial.println("\nWiFi at: " + WiFi.localIP().toString());
  delay(500);
 
  /*
    String googleSheetsSentResult = Send_Data_To_Goole_Script("TEST 2");
    Serial.println(googleSheetsSentResult);
  */
}
 
// loop
void loop() {
   sendData("data_to_send");
  delay(250000);
 
  yield();
}
 
float getFakeTemperature() {
  return micros() % 20;
}
 
String fakeFunc1() {
  return "somedata";
}
 
float fakeFunc2() {
  return millis() % 100;
}
 
void SendAlarm()//use this function to notify if something wrong (example sensor says -128C)
// don't forget to set true for enableSendingEmails in google script
{
  sendData("alarm=fixme");
}
 
/*
  HTTPClient http;
  String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + params;
  Serial.print(url);
  Serial.print(F("Making a request"));
  http.begin(url, root_ca);
  int httpCode = http.GET();
  http.end();
  Serial.println(": done " + httpCode);
*/
 
String Send_Data_To_Goole_Script(String params) {
  String resultStr;
 
  HTTPClient http;
 
  String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + params;
  Serial.println(url);
 
  if (http.begin(url, root_ca)) { // HTTPs
    //Serial.print("[HTTP] GET qualiteDeAirURL ...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();
 
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      //Serial.printf("[HTTP] GET... code: % d\n", httpCode);
 
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        resultStr = payload;
        Serial.println(payload);
      }
    } else {
      //Serial.printf("[HTTP] GET... failed, error: % s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    //Serial.printf("[HTTPUnable to connect\n");
  }
  return resultStr;
}
 
// https://script.google.com/macros/s/AKfycbxAILaeSLml-1yK5AXMxLIx8n6KtRMhIVztWGKD4KsSxsiu7Bg/exec
void sendData(String msg) {
  HTTPClient http;
  String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?";
 
  url += "msg=";
  url += urlencode(msg);
 
  Serial.println();
  Serial.println(F("--------------"));
  Serial.println(F("URL"));
  Serial.println(url);
  Serial.println(F("--------------"));
 
  Serial.print(F("\nMaking a request"));
  http.begin(url, root_ca);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.println(payload);
  }
  http.end();
 
  Serial.println(": done " + httpCode);
}