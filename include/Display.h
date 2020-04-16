#ifndef Display_h
#define Display_h

#include <ESP8266WiFi.h>
#include <Wire.h>

#include "SH1106.h"

class Display {
 private:
  SH1106* _oled;

  void init();

 public:
  void clearLine(int xOffset, int yOffset);
  void drawString(uint8_t x, uint8_t y, String str);

  Display();
};

#endif
