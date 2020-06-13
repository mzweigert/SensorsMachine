#ifndef SensorsMachineWebServer_h
#define SensorsMachineWebServer_h

#include <ArduinoJson.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <FS.h>

#include "Definitions.h"
#include "SPIFFSReadServer.h"
#include "SensorsState.h"
#include "WaterPumpsController.h"

class SensorsMachineWebServer {
 private:
  SPIFFSReadServer* _server;
  SensorsState* _sensorsState;
  WaterPumpsController* _pumpsController;

  byte clampValue(byte value, byte min, byte max);
  void initEndpoints();

 public:
  SensorsMachineWebServer(int port, SensorsState* _sensorsState, WaterPumpsController* _pumpsController);
  void loop();
};

#endif
