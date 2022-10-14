// GDSC IoT 3rd session
// GM

#include <Arduino.h>


#define DEBUG_Wifi_multi  1    // set to one for print debug wifi messages

#define ESP8266

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include "HTTPSRedirect.h"


#include <ESPHTTPClient.h>
#include <JsonListener.h>
#include <time.h>                       // time() ctime()
#include <sys/time.h>                   // struct timeval
#include <coredecls.h>                  // settimeofday_cb()

#include "SSD1306Wire.h"
#include "OLEDDisplayUi.h"
#include "Wire.h"
#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapForecast.h"
#include "WeatherStationFonts.h"
#include "WeatherStationImages.h"
#include "DHT.h"
#include "DebugMacros.h"


#define TZ              3      // (utc+) TimeZone in hours
#define DST_MN          0      // daylight savings time in minutes -- non for our local TZ

// Setup
const int UPDATE_INTERVAL_SECS = 10 * 60; // Update every 30 minutes
// Display Settings
const int I2C_DISPLAY_ADDRESS = 0x3C;

#if defined(ESP8266)
const int SDA_PIN = D2;  //4
const int SDC_PIN = D1;  //5
#else
const int SDA_PIN = 19;
const int SDC_PIN = 20;
#endif


//Open an account on https://openweathermap.org/ and get your free API key
String OPEN_WEATHER_MAP_APP_ID = "replace with your own ID";
String OPEN_WEATHER_MAP_LOCATION_ID = "179330";                       // Juja


String OPEN_WEATHER_MAP_LANGUAGE = "en";
const uint8_t MAX_FORECASTS = 4;

const boolean IS_METRIC = true;

// Months and days
const String WDAY_NAMES[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
const String MONTH_NAMES[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

/***************************
   End Settings
 **************************/
SSD1306Wire     display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);
OLEDDisplayUi   ui( &display );

OpenWeatherMapCurrentData currentWeather;
OpenWeatherMapCurrent currentWeatherClient;

OpenWeatherMapForecastData forecasts[MAX_FORECASTS];
OpenWeatherMapForecast forecastClient;

#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)
time_t now;

// flag changed in the ticker function every 10 minutes
bool readyForWeatherUpdate = false;

String lastUpdate = "--";

long timeSinceLastWUpdate = 0;

//declaring prototypes
void drawProgress(OLEDDisplay *display, int percentage, String label);
void updateData(OLEDDisplay *display);
void drawDateTime(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawTemp(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawHum(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawevent(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawCurrentWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawForecast(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawForecastDetails(OLEDDisplay *display, int x, int y, int dayIndex);
void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);
void setReadyForWeatherUpdate();

FrameCallback frames[] = { drawDateTime, drawCurrentWeather, drawForecast, drawTemp, drawHum, drawevent };
int numberOfFrames = 6;

OverlayCallback overlays[] = { drawHeaderOverlay };
int numberOfOverlays = 1;

DHT dht = DHT(12, DHT11, 6); 

//////////////////////////////////////////////////////////////////////////////////////////////

 
//Connection Settings
const char* host = "script.google.com";
const char* googleRedirHost = "script.googleusercontent.com";
const int httpsPort = 443;

unsigned long entryCalender, entryPrintStatus, entryInterrupt, heartBeatEntry, heartBeatLedEntry;
String url;


#define UPDATETIME 10000

// better way of connecting to wifi access points
// check the link below for more info
// https://github.com/GavinsMJ/uC-uP-Tricks-and-Cheats/tree/main/MultiWiFiConnect
 

//Higher Priority networks
const char* PKNOWN_SSID[] = {"net1"};                   
const char* PKNOWN_PASSWORD[] = {"net1Password"};

//Lower Priority Networks
const char* KNOWN_SSID[] = {"Net2", "net3", "net4"};
const char* KNOWN_PASSWORD[] = {"net2pass", "net3pass",  "net4pass"};

// number of known networks
const int   PKNOWN_SSID_COUNT = sizeof(PKNOWN_SSID) / sizeof(PKNOWN_SSID[0]); 
const int   KNOWN_SSID_COUNT = sizeof(KNOWN_SSID) / sizeof(KNOWN_SSID[0]); 


//Google Script ID

const char *GScriptIdRead = "replace - me"; //replace with you gscript id for reading the calendar
const char *GScriptIdWrite ="replace - me"; //replace with you gscript id for writing the calendar
 

#define NBR_EVENTS 2                 // number of events

String  possibleEvents[NBR_EVENTS] = {"Stop_Working", "Call_HOME"};
byte  LEDpins[NBR_EVENTS]    = {D4, D8};  // connect LEDs to these pins or change pin number here---      2 & 15
byte  switchPins[NBR_EVENTS] = {D0, D7};  // connect switches to these pins or change pin number here--- 16 & 13
bool switchPressed[NBR_EVENTS];
boolean beat = false;
int beatLED = 0;

enum taskStatus {           //either of three
  none,
  due,
  done
};

taskStatus taskStatus[NBR_EVENTS];
HTTPSRedirect* client = nullptr;

String calendarData = "Nothing To-Do";
bool calenderUpToDate;



void setup() {

  Serial.begin(115200);
  Serial.println("IoT Stuff \n\n");

  for (int i = 0; i < NBR_EVENTS; i++) {
    pinMode(LEDpins[i], OUTPUT);
    taskStatus[i] = none;  // Reset all LEDs
    pinMode(switchPins[i], INPUT_PULLUP);
    switchPressed[i] = false;
    attachInterrupt(digitalPinToInterrupt(switchPins[i]), handleInterrupt, FALLING);
  }

  // initialize dispaly
  display.init();
  display.clear();
  display.display();
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setContrast(255);
  
  display.clear();
  display.drawString(64, 10, "Searching for networks");
  display.display();
  
  dht.begin();

  connectToWifi();

  // Get time from network time service
  configTime(TZ_SEC, DST_SEC, "pool.ntp.org");

  ui.setTargetFPS(30);

  ui.setActiveSymbol(activeSymbole);
  ui.setInactiveSymbol(inactiveSymbole);

  // You can change this to your preferred animation effects
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_TOP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  ui.setFrames(frames, numberOfFrames);
  ui.setOverlays(overlays, numberOfOverlays);

  // Inital UI takes care of initalising the display too.
  ui.init();

  Serial.println("");

  updateData(&display);
    ///GDSC LOGO/////
    display.clear();
    display.drawXbm(0,0,logo1_width,logo1_height,logo1_bits);    //128*64   -- CHECK SITE https://diyusthad.com/image2cpp    -- for converting image to cpp 
    display.display();                                                             // OR  https://javl.github.io/image2cpp/  -- for converting image to cpp
    delay(2000);
    //////////////// 
  init_anime();

  getCalendar();
  entryCalender = millis();

  init_load();
}

void loop() {

  if (millis() - timeSinceLastWUpdate > (1000L * UPDATE_INTERVAL_SECS)) {
    setReadyForWeatherUpdate();
    timeSinceLastWUpdate = millis();
  }

  if (readyForWeatherUpdate && ui.getUiState()->frameState == FIXED) {
    updateData(&display);
  }

  int remainingTimeBudget = ui.update();

  if (remainingTimeBudget > 0) {
    delay(remainingTimeBudget);
  }

  //TASK
  if (millis() > entryCalender + UPDATETIME) {
    getCalendar();
    entryCalender = millis();
  }
  manageStatus();
  if (millis() > entryPrintStatus + 5000) {
    printStatus();
    entryPrintStatus = millis();
  }
  if (millis() > heartBeatEntry + 30000) {
    beat = true;
    heartBeatEntry = millis();
  }
  heartBeat();
}
