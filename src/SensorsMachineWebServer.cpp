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

  _server->on("/getSensorsState", [&]() {
    String response = "{ \"ip\" : \"" + WiFi.localIP().toString() + "\", \"port\" : " + WEB_SOCKET_SERVER_PORT + " }";
    _server->send(200, "application/json", _sensorsState->readAsJSON());
  });
}

SensorsMachineWebServer::SensorsMachineWebServer(int port, SensorsState* _sensorsState) {
  this->_server = new SPIFFSReadServer(port);
  this->_sensorsState = _sensorsState;
  initEndpoints();
  SPIFFS.begin();
  _server->begin();
}

void SensorsMachineWebServer::loop() {
  _server->handleClient();
}
