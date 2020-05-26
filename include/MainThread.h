
#ifndef MainThread_h
#define MainThread_h

#include "Display.h"
#include "SensorsMachineWebServer.h"
#include "SensorsState.h"
#include "ThreadController.h"
#include "WaterPumpsController.h"
#include "WebSocketsServerRunner.h"
#include "WiFiConnector.h"

class MainThread : public Thread {
 private:
  ThreadController _threads = ThreadController();
  Display* _display;
  SensorsState* _sensorsState;
  WaterPumpsController* _pumpsController;
  std::map<I2C, Sensor*> _previousConnections;
  WebSocketsServerRunner* _webSocketsServerRunner;
  SensorsMachineWebServer* _sensorsMachineWebServer;

  void initSensorsInfo();
  void initWiFiConnection();
  void initServers();

  void refreshSensorsConnectionInfoOnDisplay();
  void refreshSensorsStateInfoOnDisplay();
  void refreshServers();

 public:
  MainThread();
};

#endif
