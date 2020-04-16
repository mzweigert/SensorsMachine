
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
  WebSocketsServerRunner* _webSocketsServerRunner;

  void initSensors();
  void initWiFiConnection();
  void initWebSocketServerRunner();

  void refreshSensorsInfoOnDisplay();
  void refreshWebSocketServerRunner();
  
 public:
 
  MainThread();
};

#endif
