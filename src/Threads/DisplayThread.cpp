#include "DisplayThread.h"

#include <Wire.h>
#include <SPI.h>
#include <ESP8266WiFi.h>

#include "../Adafruit-GFX/Adafruit_GFX.h"
#include "../Adafruit_SSD1306/Adafruit_SSD1306.h"

#define D5 14
/* Object named display, of the class Adafruit_SSD1306 */
Adafruit_SSD1306 display(D5);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void DisplayThread::clearLine(int yOffset) {
  for (int y = 0; y <= 6; y++) {
    for (int x = 0; x < 127; x++) {
      display.drawPixel(x, y + yOffset, BLACK);
    }
  }
}

void DisplayThread::drawString(uint8_t x, uint8_t y, String str) {
  clearLine(y);
  display.setCursor(x, y);  /* Set x,y coordinates */
  display.println(str);
}

void DisplayThread::initDisplay() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); /* Initialize display with address 0x3C */
  display.clearDisplay();  /* Clear display */
  display.setTextSize(1.5);  /* Select font size of text. Increases with size of argument. */
  display.setTextColor(WHITE);  /* Color of text*/
  drawString(6, 10, (String) "Temperature: " + _sensorsState.readTemperature());
  drawString(6, 25, (String) "Humidity: " + _sensorsState.readHumidity());
  drawString(6, 40, (String) "Soil moisture: " + _sensorsState.readSoilMoisture());
  drawString(6, 55, (String) "WiFi Connected: " + (WiFi.status() == WL_CONNECTED ? "Yes" : "No"));
}

bool DisplayThread::checkIfStateChanged(int* last, int current, bool* anyStateChanged) {
  if (*last != current) {
    *last = current;
    *anyStateChanged = true;
    return true;
  }
  return false;
}

void DisplayThread::displaySensorsInfo() {
  bool anyStateChanged = false;

  bool temperatureChanged = checkIfStateChanged(&(DisplayThread::_lastTemperature), _sensorsState.readTemperature(), &anyStateChanged);
  if (temperatureChanged)  drawString(6, 10, (String) "Temperature: " + this->_lastTemperature);

  bool humidityChanged = checkIfStateChanged(&(DisplayThread::_lastHumidity), _sensorsState.readHumidity(), &anyStateChanged);
  if (humidityChanged) drawString(6, 25, (String) "Humidity: " + this->_lastHumidity);

  bool soilMoistureChanged = checkIfStateChanged(&(DisplayThread::_lastSoilMoisture), _sensorsState.readSoilMoisture(), &anyStateChanged);
  if (soilMoistureChanged && this->_lastSoilMoisture > 0) {
    drawString(6, 40, (String) "Soil moisture: " + this->_lastSoilMoisture);
    this->_ledRGBManager.setLightBySoilMoisture(this->_lastSoilMoisture);
  }

  bool wifiStatusChanged = checkIfStateChanged(&(DisplayThread::_lastWiFiStatus), WiFi.status() == WL_CONNECTED, &anyStateChanged);
  if (wifiStatusChanged) drawString(6, 55, (String) "WiFi Connected: " + (this->_lastWiFiStatus ? "Yes" : "No"));

  if (anyStateChanged) {
    display.display();
  }
}

DisplayThread::DisplayThread(long interval, SensorsState sensorsState) : Thread(-100, interval) {
  _sensorsState = sensorsState;
  _ledRGBManager = LedRGBManager();
  initDisplay();
  const auto thread = std::bind(&DisplayThread::displaySensorsInfo, this);
  Thread::onRun(thread);
}
