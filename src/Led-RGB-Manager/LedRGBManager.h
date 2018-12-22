#ifndef LedRGBManager_h
#define LedRGBManager_h

#include "../SensorsState/SensorsState.h"

class LedRGBManager {

  private:
    SensorsState _sensorsState;

  public:
    void setRGBLed(bool red, bool blue, bool green);
    void setLightBySoilMoisture(int soilMoisture = 0);
    LedRGBManager(SensorsState *sensorsState = NULL);
        
};

#endif
