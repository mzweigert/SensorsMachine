#ifndef DisplayThread_h
#define DisplayThread_h

#include "../SensorsState/SensorsState.h"
#include "Thread.h"

class DisplayThread : public Thread {

  private:
    SensorsState _sensorsState;
    float _lastHumidity,
          _lastTemperature,
          _lastSoilMoisture,
          _lastWiFiStatus;

    void clearLine(int yOffset);
    
    void drawString(uint8_t x, uint8_t y, String str);
    
    void initDisplay();

    float checkIfStateChanged(float* last, float current, bool* anyStateChanged);

  public:
    void displaySensorsInfo();

    DisplayThread(long interval, SensorsState sensorsState);

};

#endif
