#ifndef SensorsState_h
#define SensorsState_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class SensorsState {
  public:
  
    SensorsState();
    
    int readTemperature();
    
    int readHumidity();
    
    int readSoilMoisture();
    
    String readAsJSON();
    
};

#endif