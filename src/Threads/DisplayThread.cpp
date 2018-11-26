#include "DisplayThread.h"


#include <Wire.h>
#include <SPI.h>
#include <ESP8266WiFi.h>

#include "../Adafruit-GFX/Adafruit_GFX.h"
#include "../Adafruit_SSD1306/Adafruit_SSD1306.h"

#define OLED_RESET D5
/* Object named oledDisplay, of the class Adafruit_SSD1306 */
Adafruit_SSD1306 oledDisplay(D5);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


void DisplayThread::drawString(uint8_t x, uint8_t y, String str) {
  oledDisplay.setCursor(x, y);  /* Set x,y coordinates */
  oledDisplay.println(str);
}

void DisplayThread::initDisplay() {
  oledDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C); /* Initialize oledDisplay with address 0x3C */
  oledDisplay.clearDisplay();  /* Clear oledDisplay */
  oledDisplay.setTextSize(1.5);  /* Select font size of text. Increases with size of argument. */
  oledDisplay.setTextColor(WHITE);  /* Color of text*/
}

float DisplayThread::checkIfStateChanged(float* last, float current, bool* anyStateChanged) {
  if (*last != current) {
    *last = current;
    *anyStateChanged = true;
    return current;
  }
  return NULL;
}

void DisplayThread::displaySensorsInfo() {
  bool anyStateChanged = false;

  float temperature = checkIfStateChanged(&(DisplayThread::_lastTemperature), _sensorsState.readTemperature(), &anyStateChanged);
  if (temperature != NULL) drawString(6, 10, (String) "Temperature: " + temperature);

  float humidity = checkIfStateChanged(&(DisplayThread::_lastHumidity), _sensorsState.readHumidity(), &anyStateChanged);
  if (humidity != NULL) drawString(6, 25, (String) "Humidity: " + humidity);

  float soilMoisture = checkIfStateChanged(&(DisplayThread::_lastSoilMoisture), _sensorsState.readSoilMoisture(), &anyStateChanged);
  if (soilMoisture != NULL) drawString(6, 40, (String) "Soil moisture: " + soilMoisture);

  float wifiConnected = checkIfStateChanged(&(DisplayThread::_lastWiFiStatus), WiFi.status() == WL_CONNECTED, &anyStateChanged);
  if (wifiConnected != NULL) drawString(6, 55, (String) "WiFi Connected: " + (wifiConnected ? "Yes" : "No"));

  if (anyStateChanged) {
    oledDisplay.display();
  }
}

DisplayThread::DisplayThread(long interval, SensorsState sensorsState = SensorsState()) : Thread(-100, interval) {
  _sensorsState = sensorsState;
  initDisplay();
  drawString(6, 10, (String) "Temperature: " + _sensorsState.readTemperature());
  drawString(6, 25, (String) "Humidity: " + _sensorsState.readHumidity());
  drawString(6, 40, (String) "Soil moisture: " + _sensorsState.readSoilMoisture());
  drawString(6, 55, (String) "WiFi Connected: " + (WiFi.status() == WL_CONNECTED ? "Yes" : "No"));
  const auto thread = std::bind(&DisplayThread::displaySensorsInfo, this);
  Thread::onRun(thread);
}
