#include "MainThread.h"

void MainThread::initSensorsInfo() {
  std::map<I2C, Sensor *> newConnections = _sensorsState->getConnections();
  short offset;
  for (auto const &connection : newConnections) {
    offset = connection.first.index * 10;
    if (connection.second != NULL) {
      Sensor *sensor = connection.second;
      _display->drawString(6, offset, (String) "Sensor " + sensor->getId() + ": " + sensor->readSoilMoisture() + "%");
    } else {
      _display->drawString(6, offset, (String) "Sensor " + connection.first.index + ": not connected!");
    }
  }
  _previousConnections = newConnections;
}
void MainThread::initWiFiConnection() {
  boolean connected = WiFiConnector::connectToWiFi();
  if (connected) {
    _webSocketsServerRunner = new WebSocketsServerRunner(WEB_SOCKET_SERVER_PORT,
                                                         _sensorsState, _pumpsController,
                                                         WEB_SOCKET_SERVER_ORIGIN);
    _sensorsMachineWebServer = new SensorsMachineWebServer(WEB_SERVER_PORT,
                                                           _sensorsState, _pumpsController);
    digitalWrite(LED_BUILTIN, LOW);
  }
  _display->drawString(6, 50, (String) "WiFi Connected: " + (WiFiConnector::isConnected() ? "Yes" : "No"));
}

void MainThread::refreshSensorsConnectionInfoOnDisplay() {
  std::map<I2C, Sensor *> newConnections = _sensorsState->getConnections();
  short offset;
  for (auto const &con : newConnections) {
    if (con.second == NULL && _previousConnections[con.first] != NULL) {
      short id = _previousConnections[con.first]->getId();
      offset = con.first.index * 10;
      _display->drawString(6, offset, (String) "Sensor " + id + ": not connected!");
    } else if (con.second != NULL && _previousConnections[con.first] == NULL) {
      Sensor *sensor = con.second;
      offset = con.first.index * 10;
      _display->drawString(6, offset, (String) "Sensor " + sensor->getId() + ": " + sensor->readSoilMoisture() + "%");
    }
  }

  _previousConnections = newConnections;
}

void MainThread::refreshSensorsStateInfoOnDisplay() {
  std::map<I2C, Sensor *> connections = _sensorsState->getConnections();
  short offset;
  for (auto const &con : connections) {
    if (con.second != NULL) {
      Sensor *sensor = con.second;
      offset = con.first.index * 10;
      _display->drawString(6, offset, (String) "Sensor " + sensor->getId() + ": " + sensor->readSoilMoisture() + "%");
    }
  }
  _previousConnections = connections;
}

void MainThread::refreshServers() {
  WiFiConnector::loop();
  if (WiFiConnector::isConnected()) {
    if (_webSocketsServerRunner == NULL) {
      _webSocketsServerRunner = new WebSocketsServerRunner(WEB_SOCKET_SERVER_PORT,
                                                           _sensorsState, _pumpsController,
                                                           WEB_SOCKET_SERVER_ORIGIN);
    }
    if (_sensorsMachineWebServer == NULL) {
      _sensorsMachineWebServer = new SensorsMachineWebServer(WEB_SERVER_PORT,
                                                             _sensorsState, _pumpsController);
      digitalWrite(LED_BUILTIN, LOW);
    }
    _webSocketsServerRunner->loop();
    _sensorsMachineWebServer->loop();
  }
  _display->drawString(6, 50, (String) "WiFi Connected: " + (WiFiConnector::isConnected() ? "Yes" : "No"));
}

MainThread::MainThread() : Thread(0, 0) {
  _display = new Display();
  _sensorsState = new SensorsState();
  _pumpsController = new WaterPumpsController(_sensorsState);
  initSensorsInfo();
  initWiFiConnection();

  Thread *refreshSensorsConnection = new Thread(1, REFRESH_SENSORS_CONNECTIONS_INTERVAL,
                                                std::bind(&MainThread::refreshSensorsConnectionInfoOnDisplay, this));
  _threads.add(refreshSensorsConnection);

  Thread *refreshSensorsInfo = new Thread(2, REFRESH_SENSORS_INFO_INTERVAL,
                                          std::bind(&MainThread::refreshSensorsStateInfoOnDisplay, this));
  _threads.add(refreshSensorsInfo);

  Thread *refreshWebSocketServerRunner = new Thread(3, 0, std::bind(&MainThread::refreshServers, this));
  _threads.add(refreshWebSocketServerRunner);

  Thread *pumpsControllerLoop = new Thread(4, 0, std::bind(&WaterPumpsController::loop, _pumpsController));
  _threads.add(pumpsControllerLoop);

  auto const run = std::bind(&ThreadController::loop, &_threads);
  Thread::onRun(run);
}
