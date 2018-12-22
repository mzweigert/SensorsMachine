#ifndef DisplayThread_h
#define DisplayThread_h

#include "../SensorsState/SensorsState.h"
#include "../Led-RGB-Manager/LedRGBManager.h"
#include "Thread.h"

class DisplayThread : public Thread {

  private:
    SensorsState _sensorsState;
    LedRGBManager _ledRGBManager;
    int _lastHumidity,
        _lastTemperature,
        _lastSoilMoisture,
        _lastWiFiStatus;

    void clearLine(int yOffset);
    
    void drawString(uint8_t x, uint8_t y, String str);
    
    void initDisplay();

    bool checkIfStateChanged(int* last, int current, bool* anyStateChanged);

  public:
    void displaySensorsInfo();

    DisplayThread(long interval, SensorsState sensorsState = SensorsState());

};

#endif
