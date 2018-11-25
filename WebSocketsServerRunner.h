
#ifndef WEBSOCKETSSERVERRUNNER_H_
#define WEBSOCKETSSERVERRUNNER_H_

#include "src/Threads/ConsumerController.h"
#include "src/WebSockets/WebSocketsServer.h"
#include "SensorsState.h"


class WebSocketsServerRunner : public WebSocketsServer {
  private:
    ConsumerController<uint8_t> _consumers = ConsumerController<uint8_t>();
    SensorsState _sensorsState;

    void notifyClient(uint8_t clientNumber) {
      String message = _sensorsState.readAsJSON();
      WebSocketsServer::sendTXT(clientNumber, message);
      Serial.println((String) "Consumer run : " + clientNumber);
    }

    void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

      switch (type) {
        case WStype_DISCONNECTED: {
            Serial.printf("[%u] Disconnected!\n", num);
            _consumers.stop(static_cast<int>(num));
          }
          break;
        case WStype_CONNECTED: {

            IPAddress ip = WebSocketsServer::remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
            auto const notifyClient = std::bind(&WebSocketsServerRunner::notifyClient, this, std::placeholders::_1);
            Consumer<uint8_t>* consumer = new Consumer<uint8_t>(static_cast<int>(num), notifyClient, num, 5000);
            _consumers.add(consumer);
          }
          break;
      }
    }

  public:

    WebSocketsServerRunner(uint16_t port, String origin = "", SensorsState sensorsState = SensorsState(), String protocol = "arduino") : WebSocketsServer(port, origin, protocol) {
      _consumers = ConsumerController<uint8_t>();
      _sensorsState = sensorsState;
      const auto event = std::bind(&WebSocketsServerRunner::webSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
      WebSocketsServer::onEvent(event);
    }

    virtual void loop() {
      WebSocketsServer::loop();
      _consumers.run();
    }
};


#endif /* WEBSOCKETSSERVERCONFIG_H_ */
