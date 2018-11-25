#ifndef DisplayThread_h
#define DisplayThread_h

#include <Wire.h>
#include <SPI.h>
#include <ESP8266WiFi.h>

#include "SensorsState.h"
#include "src/Threads/Thread.h"
#include "src/Adafruit-GFX/Adafruit_GFX.h"
#include "src/Adafruit_SSD1306/Adafruit_SSD1306.h"

#define OLED_RESET D5
/* Object named display, of the class Adafruit_SSD1306 */
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


class DisplayThread : public Thread {

  private:
    SensorsState _sensorsState;
    float _lastHumidity,
          _lastTemperature,
          _lastSoilMoisture,
          _lastWiFiStatus;

    void drawString(uint8_t x, uint8_t y, String str) {
      display.setCursor(x, y);  /* Set x,y coordinates */
      display.println(str);
    }

    void initDisplay() {
      display.begin(SSD1306_SWITCHCAPVCC, 0x3C); /* Initialize display with address 0x3C */
      display.clearDisplay();  /* Clear display */
      display.setTextSize(1.5);  /* Select font size of text. Increases with size of argument. */
      display.setTextColor(WHITE);  /* Color of text*/
    }

    float checkIfStateChanged(float* last, float current, bool* anyStateChanged) {
      if (*last != current) {
        *last = current;
        *anyStateChanged = true;
        return current;
      }
      return NULL;
    }

  public:

    void displaySensorsInfo() {
      bool anyStateChanged = false;

      float temperature = checkIfStateChanged(&(DisplayThread::_lastTemperature), _sensorsState.readTemperature(), &anyStateChanged);
      if (temperature != NULL) drawString(6, 10, (String) "Temperature: " + temperature);

      float* lastHumidityPtr = &(DisplayThread::_lastHumidity);
      float humidity = checkIfStateChanged(&(DisplayThread::_lastHumidity), _sensorsState.readHumidity(), &anyStateChanged);
      if (humidity != NULL) drawString(6, 25, (String) "Humidity: " + humidity);

      float soilMoisture = checkIfStateChanged(&(DisplayThread::_lastSoilMoisture), _sensorsState.readSoilMoisture(), &anyStateChanged);
      if (soilMoisture != NULL) drawString(6, 40, (String) "Soil moisture: " + soilMoisture);

      float wifiConnected = checkIfStateChanged(&(DisplayThread::_lastWiFiStatus), WiFi.status() == WL_CONNECTED, &anyStateChanged);
      if (wifiConnected != NULL) drawString(6, 55, (String) "WiFi Connected: " + (wifiConnected ? "Yes" : "No"));

      if (anyStateChanged) {
        display.display();
      }
    }

    DisplayThread(long interval, SensorsState sensorsState = SensorsState()) : Thread(-100, interval) {
      _sensorsState = sensorsState;
      initDisplay();
      drawString(6, 10, (String) "Temperature: " + _sensorsState.readTemperature());
      drawString(6, 25, (String) "Humidity: " + _sensorsState.readHumidity());
      drawString(6, 40, (String) "Soil moisture: " + _sensorsState.readSoilMoisture());
      drawString(6, 55, (String) "WiFi Connected: " + (WiFi.status() == WL_CONNECTED ? "Yes" : "No"));
      const auto thread = std::bind(&DisplayThread::displaySensorsInfo, this);
      Thread::onRun(thread);
    }

    void loop() {
      if (this->shouldRun()) {
        this->run();
      }
    }
};

#endif
