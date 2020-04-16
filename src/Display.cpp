#include "Display.h"

#define SDA_PIN D2
#define SCL_PIN D1

void Display::init() {

  Serial.println("Initializing display...");
  Wire.begin(SDA_PIN, SCL_PIN);
  _oled = new SH1106(0x3c, SDA_PIN, SCL_PIN);
  _oled->init();
  //_oled->flipScreenVertically();
  _oled->setFont(ArialMT_Plain_10); /* Select font size of text. Increases with size of argument. */
  _oled->clear();                   /* Clear display */
}

void Display::clearLine(int xOffset, int yOffset) {
  for (int y = yOffset; y <= yOffset + 12; y++) {
    for (int x = xOffset; x < 127; x++) {
      _oled->clearPixel(x, y);
    }
  }
}

void Display::drawString(uint8_t x, uint8_t y, String str) {
  Wire.begin(SDA_PIN, SCL_PIN);
  clearLine(x, y);
  _oled->drawString(x, y, str);
  _oled->display();
}

Display::Display() {
  init();
}
