
#ifndef MainThread_h
#define MainThread_h

#include "Display.h"
#include "SensorsState.h"
#include "ThreadController.h"
#include "WebSocketsServerRunner.h"
#include "WiFiConnector.h"

class MainThread : public Thread {
 private:
  ThreadController _threads = ThreadController();
  Display* _display;
  SensorsState* _sensorsState;
  std::map<I2C, Sensor*> _previousConnections;
  WebSocketsServerRunner* _webSocketsServerRunner;

  void initSensorsInfo();
  void initWiFiConnection();
  void initWebSocketServerRunner();

  void refreshSensorsConnectionInfoOnDisplay();
  void refreshSensorsStateInfoOnDisplay();
  void refreshWebSocketServerRunner();

 public:
  MainThread();
};

#endif
