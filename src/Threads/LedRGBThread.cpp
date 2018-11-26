#include "../SensorsState/SensorsState.h"

#include "LedRGBThread.h"

const int RED = 12;
const int GREEN = 13;
const int BLUE = 15;

void LedRGBThread::setRGBLed(bool red, bool blue, bool green) {
  digitalWrite(RED, red ? LOW : HIGH);
  digitalWrite(BLUE, blue ? LOW : HIGH);
  digitalWrite(GREEN, green ? LOW : HIGH);
}

void LedRGBThread::setLightBySoilMoisture() {
  int soilMoisture = sensorsState.readSoilMoisture();
  if (soilMoisture >= 66) {
    setRGBLed(false, false, true);
  } else if (soilMoisture < 66 && soilMoisture > 33) {
    setRGBLed(false, true, false);
  } else {
    setRGBLed(true, false, false);
  }
}

LedRGBThread::LedRGBThread(long interval, SensorsState _sensorsState = SensorsState()) : Thread(-100, interval) {
  const auto thread = std::bind(&LedRGBThread::setLightBySoilMoisture, this);
  Thread::onRun(thread);
  sensorsState = _sensorsState;
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  setRGBLed(false, false, false);
}
