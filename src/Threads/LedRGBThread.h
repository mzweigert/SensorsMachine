#ifndef LedRGBThread_h
#define LedRGBThread_h

#include "../SensorsState/SensorsState.h"
#include "Thread.h"

class LedRGBThread : public Thread {

  private:
    SensorsState sensorsState;

  public:
    void setRGBLed(bool red, bool blue, bool green);

    void setLightBySoilMoisture();

    LedRGBThread(long interval, SensorsState _sensorsState);
    
};

#endif
