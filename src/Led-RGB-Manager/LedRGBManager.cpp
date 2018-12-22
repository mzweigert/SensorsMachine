#include "../SensorsState/SensorsState.h"

#include "LedRGBManager.h"

const int RED = 12;
const int GREEN = 13;
const int BLUE = 15;

void LedRGBManager::setRGBLed(bool red, bool blue, bool green) {
  digitalWrite(RED, red ? LOW : HIGH);
  digitalWrite(BLUE, blue ? LOW : HIGH);
  digitalWrite(GREEN, green ? LOW : HIGH);
}

void LedRGBManager::setLightBySoilMoisture(int soilMoisture) {
  if(soilMoisture == 0) {
      soilMoisture = _sensorsState.readSoilMoisture();
  }
  if (soilMoisture > 0 && soilMoisture < 33) {
    setRGBLed(true, false, false);
  } else if (soilMoisture < 66 && soilMoisture > 33) {
    setRGBLed(false, true, false);
  } else if (soilMoisture >= 66) {
    setRGBLed(false, false, true);
  }
}

LedRGBManager::LedRGBManager(SensorsState *sensorsState) {
  _sensorsState = *sensorsState;
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  setRGBLed(false, false, false);
}
