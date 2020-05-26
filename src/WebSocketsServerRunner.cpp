#include "WebSocketsServerRunner.h"

void WebSocketsServerRunner::notifyClient(uint8_t clientNumber) {
  String sensors = _sensorsState->readAsJSON();
  String pumps = _waterPumpsController->readAsJSON();
  String message = "{ \"sensors\" : " + sensors + ", \"pumps\" : " + pumps + " }";
  bool sent = WebSocketsServer::sendTXT(clientNumber, message);
  if (sent) {
    Serial.println((String) "Consumer ran : " + clientNumber);
  } else {
    Serial.println((String) "Message hasn't been sent : " + clientNumber);
  }
}

void WebSocketsServerRunner::webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED: {
      Serial.printf("[%u] Disconnected!\n", num);
      _consumers.stop(static_cast<int>(num));
    } break;
    case WStype_CONNECTED: {
      IPAddress ip = WebSocketsServer::remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0],
                    ip[1], ip[2], ip[3], payload);
      auto const notifyClient = std::bind(&WebSocketsServerRunner::notifyClient, this, std::placeholders::_1);
      Consumer<uint8_t>* consumer = new Consumer<uint8_t>(static_cast<int>(num), notifyClient, num);
      _consumers.add(consumer);
    } break;
  }
}

WebSocketsServerRunner::WebSocketsServerRunner(uint16_t port,
                                               SensorsState* sensorsState,
                                               WaterPumpsController* waterPumpsController,
                                               String origin, String protocol)
    : WebSocketsServer(port, origin, protocol) {
  _consumers = ConsumerController<uint8_t>(5000);
  _sensorsState = sensorsState;
  _waterPumpsController = waterPumpsController;
  const auto event = std::bind(&WebSocketsServerRunner::webSocketEvent, this,
                               std::placeholders::_1, std::placeholders::_2,
                               std::placeholders::_3, std::placeholders::_4);
  WebSocketsServer::onEvent(event);
  begin();
}

void WebSocketsServerRunner::loop() {
  _consumers.loop();
  WebSocketsServer::loop();
}
