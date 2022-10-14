// Variadic macros used to print information in de-bugging mode 

#pragma once
// un-comment this line to print the debugging statements
//#define DEBUG

#ifdef DEBUG
  #define DPRINT(...)    Serial.print(__VA_ARGS__)
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)
#else
  // define blank line
  #define DPRINT(...)
  #define DPRINTLN(...)
#endif


#if DEBUG_Wifi_multi == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

