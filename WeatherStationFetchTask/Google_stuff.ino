// By GM
void printStatus() {
  for (int i = 0; i < NBR_EVENTS; i++) {
    Serial.print("Task ");
    Serial.print(i);
    Serial.print(" Status ");
    Serial.println(taskStatus[i]);
  }
  Serial.println("----------");
}

void getCalendar() {
  //  Serial.println("Start Request");
  // HTTPSRedirect client(httpsPort);
  unsigned long getCalenderEntry = millis();

  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    ESP.reset();
  }
  //Fetch Google Calendar events
  String url = String("/macros/s/") + GScriptIdRead + "/exec";
  client->GET(url, host);
  calendarData = client->getResponseBody();
  Serial.print("Calendar Data---> ");
  Serial.println(calendarData);
  calenderUpToDate = true;
  yield();
}

void createEvent(String title) {
  // Serial.println("Start Write Request");

  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    ESP.reset();
  }
  // Create event on Google Calendar
  String url = String("/macros/s/") + GScriptIdWrite + "/exec" + "?title=" + title;
  client->GET(url, host);
  //  Serial.println(url);
  Serial.println("Write Event created ");
  calenderUpToDate = false;
}

 // SEPARATE MULTIPLE TASKS 
void separate_tasks(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y){
  int index = 0;int count = 0;int pos = 15;int i1;int i2;
  String del = ";";  // delimiter
   
    // get number of tasks
    for(int i = 0; i < calendarData.length(); i++ ){
       if (calendarData[i] == del[0]) count ++; }

    // 1st task
    i1 = calendarData.indexOf(';');
    String Task_ = calendarData.substring(0, i1);
    display->drawString(64 + x, pos + y, Task_);
    //Serial.print("first :: ");Serial.println(Task_);
    
    // separate and display subsequent taks
    for(int i = 0; i < count; i++ ){
      pos += 10; 
      if(i == (count - 1)){
          //last task
         Task_ = calendarData.substring(i1 + 1);
         display->drawString(64 + x, pos + y, Task_);
         display->setTextAlignment(TEXT_ALIGN_LEFT);    
         //Serial.print("fin :: ");Serial.println(Task_);
         return;
       }
       i2    = calendarData.indexOf(';', i1+1);    
       Task_ = calendarData.substring(i1 + 1, i2);
       display->drawString(64 + x, pos + y, Task_); 
       //Serial.print("mid :: ");Serial.println(Task_);
       i1 = i2;      
    }
 }


void drawevent(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {        // draw current google calender event 
 
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(64 + x, 5 + y, "Current Task:");
 
  // set font size depending on data length
  int event_len =calendarData.length();
  if(event_len <= 10 )
    display->setFont(ArialMT_Plain_24);
  if(event_len > 10  && event_len <= 20 )
    display->setFont(ArialMT_Plain_16);
  if(event_len > 20 )
    display->setFont(ArialMT_Plain_10);

  // display
  if(calendarData.indexOf(";") >= 0  ){
     // We have more than one task
     // Multiple tasks are delimited by semicolon  -- separate and display task
     separate_tasks(display, state, x, y);
     
   }else{  
      display->drawString(64 + x, 15 + y, String(calendarData));
      display->setTextAlignment(TEXT_ALIGN_LEFT);     
   }

}


void manageStatus() {
  for (int i = 0; i < NBR_EVENTS; i++) {
    switch (taskStatus[i]) {
      case none:
        if (switchPressed[i]) {                              // SWITCH PRESSED
          digitalWrite(LEDpins[i], HIGH);                    // TURN ON CORRESPONDING LED
          while (!calenderUpToDate) getCalendar();           // UPDATE CALENDER
          if (!eventHere(i)) createEvent(possibleEvents[i]); // CREATE EVENT
          Serial.print(i);
          
          Serial.println(" 0 -->1");
          //getCalendar();
          taskStatus[i] = due;
        } else {
          if (eventHere(i)) {
            digitalWrite(LEDpins[i], HIGH);
            Serial.print(i);
            Serial.println(" 0 -->1");
            taskStatus[i] = due;
          }
        }
        break;
      case due:
        if (switchPressed[i]) {
          digitalWrite(LEDpins[i], LOW);
          Serial.print(i);
          Serial.println(" 1 -->2");
          taskStatus[i] = done;
        }
        break;
      case done:
        if (calenderUpToDate && !eventHere(i)) {
          digitalWrite(LEDpins[i], LOW);
          Serial.print(i);
          Serial.println(" 2 -->0");
          taskStatus[i] = none;
        }
        break;
      default:
        break;
    }
    switchPressed[i] = false;
  }
  yield();
}

bool eventHere(int task) {
  if (calendarData.indexOf(possibleEvents[task], 0) >= 0 ) {
    //    Serial.print("Task found ");
    //    Serial.println(task);
    return true;
  } else {
    //   Serial.print("Task not found ");
    //   Serial.println(task);
    return false;
  }
}

ICACHE_RAM_ATTR void handleInterrupt() {
  if (millis() > entryInterrupt + 100) {
    entryInterrupt = millis();
    for (int i = 0; i < NBR_EVENTS; i++) {
      if (digitalRead(switchPins[i]) == LOW) {
        switchPressed[i] = true;
      }
    }
  }
}
