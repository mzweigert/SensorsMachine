#include "SensorsMachineWebServer.h"

void SensorsMachineWebServer::initEndpoints() {
  _server->on("/", [&]() {
    if (!_server->handleFileRead("/index.html"))
      _server->send(404, "text/plain", "404: Not Found");
  });

  _server->on("/getWebSocketServerAddress", [&]() {
    String response = "{ \"ip\" : \"" + WiFi.localIP().toString() + "\", \"port\" : " + WEB_SOCKET_SERVER_PORT + " }";
    _server->send(200, "application/json", response);
  });

  _server->on("/getMachineInfo", [&]() {
    String sensors = _sensorsState->readAsJSON();
    String pumps = _pumpsController->readAsJSON();
    String response = "{ \"sensors\" : " + sensors + ", \"pumps\" : " + pumps + " }";
    _server->send(200, "application/json", response);
  });

  _server->on("/runPump", HTTP_POST, [&]() {
    StaticJsonDocument<200> doc;
    deserializeJson(doc, _server->arg("plain"));
    byte number = doc["number"];
    ProcessStatus status = _pumpsController->runWateringProcess(number);
    _server->send(status == PUMP_HAS_RUNNED ? 200 : 400,
                  "application/json",
                  (String) "{ \"number\" : " + number + ", \"status\" : \"" + status + "\" }");
  });
}

SensorsMachineWebServer::SensorsMachineWebServer(int port, SensorsState* _sensorsState,
                                                 WaterPumpsController* _pumpsController) {
  this->_server = new SPIFFSReadServer(port);
  this->_sensorsState = _sensorsState;
  this->_pumpsController = _pumpsController;
  initEndpoints();
  SPIFFS.begin();
  _server->begin();
}

void SensorsMachineWebServer::loop() {
  _server->handleClient();
}
