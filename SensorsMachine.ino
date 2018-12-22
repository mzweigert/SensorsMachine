#include "src/WiFiManagement/WiFiConnector.h"
#include "src/WebSockets/WebSocketsServerRunner.h"
#include "src/SensorsState/SensorsState.h"
#include "src/Threads/DisplayThread.h"

SensorsState *sensorsState;
WebSocketsServerRunner *webSocketsServerRunner;
DisplayThread *displayThread;

void initWebSocketServerRunner() {
  webSocketsServerRunner = new WebSocketsServerRunner(8080, "sensors", *sensorsState);
  webSocketsServerRunner->begin();
  digitalWrite(LED_BUILTIN, LOW);
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  sensorsState = new SensorsState();
  boolean connected = WiFiConnector::connectToWiFi();
  if (connected) {
    initWebSocketServerRunner();
  }
  displayThread = new DisplayThread(1000, *sensorsState);

}

void loop() {
  displayThread->loop();
  WiFiConnector::loop();
  if (WiFiConnector::isConnected()) {
    if (webSocketsServerRunner == NULL) {
      initWebSocketServerRunner();
    }
    webSocketsServerRunner->loop();
  }
}
