

//3 seconds WDT
#define WDT_TIMEOUT 3



    void connect_wifi(){

        bool wifiFound = false;
        bool PwifiFound = false;
        int n;
        int i;

        // WiFi.scanNetworks will return the number of networks found
        debugln(F("WIFI_Conn:: scan start"));
        int nbVisibleNetworks = WiFi.scanNetworks();
        debugln(F("WIFI_Conn:: scan done"));
        if (nbVisibleNetworks == 0) {
            debugln(F("WIFI_Conn:: no networks found. Reset to try again"));
            
            display.clear();
            display.drawString(64, 10, "No networks Found !!!");
            display.drawString(64, 20, "Rebooting ...");
            display.display();
  
            delay(1000);
            ESP.restart();
            //esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
            //esp_task_wdt_add(NULL); //add current thread to WDT watch
            //while (true); // no need to go further, hang in there, will auto launch the Soft WDT reset
        }

        // if you arrive here at least some networks are visible
        debug(nbVisibleNetworks);
        debugln("WIFI_Conn::  network(s) found");

        // check if we recognize one by comparing the visible networks
        // one by one with our list of known networks
        for (i = 0; i < nbVisibleNetworks; ++i) {
            debugln(WiFi.SSID(i)); // Print current SSID

            for (n = 0; n < PKNOWN_SSID_COUNT; n++) { // walk through the list of known SSID and check for a match

            if (strcmp(PKNOWN_SSID[n], WiFi.SSID(i).c_str())) {
                debug(F("\tWIFI_Conn:: Not matching PRIORITY "));
                debugln(PKNOWN_SSID[n]);
                PwifiFound = true;
            } else { // we got a match
                wifiFound = true;
                PwifiFound = !true;
                break; // n is the network index we found
            }

            } // end for each known wifi SSID

            if (wifiFound) break; // break from the "for each visible network" loop
        } // end for each visible network


            if (PwifiFound == !false){

                for (i = 0; i < nbVisibleNetworks; ++i) {
                debugln(WiFi.SSID(i)); // Print current SSID

                for (n = 0; n < KNOWN_SSID_COUNT; n++) { // walk through the list of known SSID and check for a match

                    if (strcmp(KNOWN_SSID[n], WiFi.SSID(i).c_str())) {
                    debug(F("\tWIFI_Conn:: Not matching "));
                    debugln(KNOWN_SSID[n]);
                    } else { // we got a match
                    wifiFound = true;
                    break; // n is the network index we found
                    }

                } // end for each known wifi SSID
                
                if (wifiFound) break; // break from the "for each visible network" loop
            } // end for each visible network

            }

        if (!wifiFound) {
            debugln(F("WIFI_Conn:: no Known network identified. Reset to try again"));
            display.clear();
            display.drawString(64, 10, "No known networks Found");
            display.drawString(64, 20, "Rebooting ...");
            display.display();
  
            delay(5000);
            ESP.restart();
            //esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
            //esp_task_wdt_add(NULL); //add current thread to WDT watch
            //while (true); // no need to go further, hang in there, will auto launch the Soft WDT reset
        }


            if (PwifiFound == false){
            // if you arrive here you found 1ST PRIORITY known SSID
            debug(F("\nWIFI_Conn:: PRIORITY FOUND \n\n\tConnecting to "));
            debugln(PKNOWN_SSID[n]);
            // We try to connect to the WiFi network we found
            WiFi.begin(PKNOWN_SSID[n], PKNOWN_PASSWORD[n]);
            }else{
            // if you arrive here you found 1 known SSID
            debug(F("\nWIFI_Conn:: PRIORITY NOT FOUND \n\n\tConnecting to "));
            debugln(KNOWN_SSID[n]);
            // We try to connect to the WiFi network we found
            WiFi.begin(KNOWN_SSID[n], KNOWN_PASSWORD[n]);
            }
        
           int counter = 0;
           
           while (WiFi.status() != WL_CONNECTED) {
             delay(500);
             Serial.print(".");
                 display.clear();
             display.drawString(64, 10, "Connecting to WiFi");
             display.drawXbm(46, 30, 8, 8, counter % 3 == 0 ? activeSymbole : inactiveSymbole);
             display.drawXbm(60, 30, 8, 8, counter % 3 == 1 ? activeSymbole : inactiveSymbole);
             display.drawXbm(74, 30, 8, 8, counter % 3 == 2 ? activeSymbole : inactiveSymbole);
             display.display();
             counter++;
           }

        debugln("");

        // ----------------------------------------------------------------
        // SUCCESS, you are connected to the known WiFi network
        // ----------------------------------------------------------------
        // debugln(F("WiFi connected, your IP address is "));
        // debugln(WiFi.localIP());
        return;
    }


    bool init_wifi() {

        WiFi.mode(WIFI_STA);

        connect_wifi();

        debugln("");
        debugln("WIFI_Conn:: WiFi connected");
        debug("WIFI_Conn:: IP address: ");
        debugln(WiFi.localIP());
        return true;

    }


//Connect to wifi
void connectToWifi() {
  Serial.println();
  Serial.println("Connecting to wifi");
 
  init_wifi();

   Serial.println("");
   Serial.print("WiFi connected ");
   Serial.print("IP address: ");
   Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");

  Serial.print("Connecting to ");
  Serial.println(host);

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
  Serial.println("Connected to Google");
}
