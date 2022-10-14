# Weather-Task-Reminder
Weather station that links to your google calendar and fetches all your tasks to show on Oled display
App shows all your events for the day on the display. It also shows the weather for the day and the time.

## Calendar Events
deployed appscript fetches all your tasks for the day, also creates a new event for the day and adds it to your calendar when a command from the device is sent. 
### Setup

1. Create a new project in google apps script
2. Copy the 'ReadEvents' code from the appscript file in this repo
3. Create a new calendar in your google calendar
4. Modify the code to add the calendar id of the new calendar you created
5. Deploy the appscript as a web app

Repeat the same steps for the 'CreateEvent' code in the appscript file

Snippets of what to change in both instances.

In the 'ReadEvents' code :

```javascript 
  // Replace MySchedules with your calendar name
  function GetEvents(){   
    var _calendarName = 'MySchedules'  
    ....
```

In the 'CreateEvent' code :

```javascript 
  // Replace MySchedules with your calendar name
  function CreateEvent(eve){
    var _calendarName = 'MySchedules'  
    ....
```

### Circuit 

Setup the cuircuit as shown in the image below

![Circuit](/Assets/Circuit/CIRCUIT_bb.png)

### Firmware

1. Clone the repo
2. Open the project in Arduino IDE and install the ESP8266 in board manager
3. Install the following libraries
    - Adafruit_SSD1306
    - Adafruit_GFX    
    - DHT sensor library 

4. Modify the code to add your wifi credentials and the url of the deployed appscript

```c++

// Open an account on https://openweathermap.org/ and get your free API key

// modify the following lines to add your open weather map id and location
String OPEN_WEATHER_MAP_APP_ID = "replace_me";
String OPEN_WEATHER_MAP_LOCATION_ID = "179330";                       //juja

// change language of weather description
String OPEN_WEATHER_MAP_LANGUAGE = "en";
const uint8_t MAX_FORECASTS = 4;

// metric / imperial ?
const boolean IS_METRIC = true;

// Months and days -- change this to your prefered language
const String WDAY_NAMES[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
const String MONTH_NAMES[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};


// Replace with your wifi credentials  -- CHECK : https://github.com/GavinsMJ/uC-uP-Tricks-and-Cheats/tree/main/MultiWiFiConnect    for more information
// wifi access points

//Higher Priority networks
const char* PKNOWN_SSID[] = {"net1"};                     // Replace with your network names
const char* PKNOWN_PASSWORD[] = {"net1Password"};

//Lower Priority Networks
const char* KNOWN_SSID[] = {"Net2", "net3", "net4"};      // Replace with your network names
const char* KNOWN_PASSWORD[] = {"net2pass", "net3pass",  "net4pass"};

// number of known networks
const int   PKNOWN_SSID_COUNT = sizeof(PKNOWN_SSID) / sizeof(PKNOWN_SSID[0]); 
const int   KNOWN_SSID_COUNT = sizeof(KNOWN_SSID) / sizeof(KNOWN_SSID[0]); 

// Replace with the Google Script ID of the deployed appscripts
//Google Script ID

const char *GScriptIdRead = "replace - me"; //replace with you gscript id for reading the calendar
const char *GScriptIdWrite ="replace - me"; //replace with you gscript id for writing the calendar

```


For custom logo visit https://javl.github.io/image2cpp/ and convert your image to c++ array

Final image of the device in action

![Device](/Assets/images/START.jpeg)
