#include "MainThread.h"

void MainThread::initSensors() {
  unsigned long startMillis;
  unsigned long currentMillis;
  const unsigned long period = 10;

  int yOffset = 0, i = 1;

  do {

    _display->drawString(6, yOffset, (String) "Sensor " + i + ": connecting... ");
    Sensor *sensor = NULL;
    startMillis = millis();
    do {

      currentMillis = millis();
      if (this->_sensorsState->isAnyConnected()) {

        _display->drawString(115, yOffset, (String)(period - (currentMillis - startMillis) / 1000));
        if (currentMillis - startMillis > period * 1000) {
          delay(500);
          _display->clearLine(0, yOffset);
          return;
        }
      }

      sensor = this->_sensorsState->connectNext();
      delay(500);
    } while (sensor == NULL);

    _display->drawString(6, yOffset, (String) "Sensor " + sensor->getId() + ": connected!");
    i++;
    yOffset += 10;

  } while (!this->_sensorsState->isAllConnected());
}

void MainThread::initWiFiConnection() {
  boolean connected = WiFiConnector::connectToWiFi();
  if (connected) {
    initWebSocketServerRunner();
  }
  _display->drawString(6, 50, (String) "WiFi Connected: " + (WiFiConnector::isConnected() ? "Yes" : "No"));
}

void MainThread::initWebSocketServerRunner() {
  _webSocketsServerRunner = new WebSocketsServerRunner(8080, _sensorsState, "sensors");
  _webSocketsServerRunner->begin();
  digitalWrite(LED_BUILTIN, LOW);
}

void MainThread::refreshSensorsInfoOnDisplay() {
  std::vector<Sensor> sensors = _sensorsState->sensors;
  short yOffset = 0;
  for (int i = 0; i < sensors.size(); i++) {

    Sensor sensor = sensors.at(i);
    _display->drawString(6, yOffset, (String) "Sensor " + sensor.getId() + ": " + sensor.readData() + "%");

    yOffset += 10;
  }
}

void MainThread::refreshWebSocketServerRunner() {
  if (WiFiConnector::isConnected()) {
    WiFiConnector::loop();
    if (_webSocketsServerRunner == NULL) {
      initWebSocketServerRunner();
    }
    _webSocketsServerRunner->loop();
  }
  _display->drawString(6, 50, (String) "WiFi Connected: " + (WiFiConnector::isConnected() ? "Yes" : "No"));
}

MainThread::MainThread() : Thread(0, 0) {
  _display = new Display();
  _sensorsState = new SensorsState();
  initSensors();
  initWiFiConnection();

  Thread *refreshSensorsInfo = new Thread(1, 5000, std::bind(&MainThread::refreshSensorsInfoOnDisplay, this));
  _threads.add(refreshSensorsInfo);

  Thread *refreshWebSocketServerRunner = new Thread(2, 1000, std::bind(&MainThread::refreshWebSocketServerRunner, this));
  _threads.add(refreshWebSocketServerRunner);

  auto const run = std::bind(&ThreadController::run, &_threads);
  Thread::onRun(run);
}
