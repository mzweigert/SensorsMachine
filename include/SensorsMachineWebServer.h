#ifndef SensorsMachineWebServer_h
#define SensorsMachineWebServer_h

#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <FS.h>

#include "Definitions.h"
#include "SPIFFSReadServer.h"
#include "SensorsState.h"

class SensorsMachineWebServer {
 private:
  SPIFFSReadServer* _server;
  SensorsState* _sensorsState;

  void initEndpoints();

 public:
  SensorsMachineWebServer(int port, SensorsState* _sensorsState);
  void loop();
};

#endif
