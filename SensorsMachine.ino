#include "WiFiManager.h"
#include "WebSocketsServerRunner.h"
#include "SensorsState.h"
#include "LedRGBThread.h"
#include "DisplayThread.h"

SensorsState *sensorsState;
WebSocketsServerRunner *webSocketsServerRunner;
LedRGBThread *ledRGBThread;
DisplayThread *displayThread;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  sensorsState = new SensorsState();
  boolean connected = WiFiManager::connectToWiFi();
  if (connected) {
    webSocketsServerRunner = new WebSocketsServerRunner(8080, "sensors", *sensorsState);
    webSocketsServerRunner->begin();
    digitalWrite(LED_BUILTIN, LOW);
  }
  ledRGBThread = new LedRGBThread(1000, *sensorsState);
  displayThread = new DisplayThread(1000, *sensorsState);

}

void loop() {
  ledRGBThread->loop();
  displayThread->loop();
  if (WiFiManager::isConnected() && webSocketsServerRunner != NULL) {
    webSocketsServerRunner->loop();
  }
}
