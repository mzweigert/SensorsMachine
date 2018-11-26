
#ifndef WEBSOCKETSSERVERRUNNER_H_
#define WEBSOCKETSSERVERRUNNER_H_

#include "../SensorsState/SensorsState.h"
#include "../Threads/ConsumerController.h"
#include "WebSocketsServer.h"


class WebSocketsServerRunner : public WebSocketsServer {
  private:
    ConsumerController<uint8_t> _consumers = ConsumerController<uint8_t>();
    SensorsState _sensorsState;

    void notifyClient(uint8_t clientNumber);

    void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

  public:

    WebSocketsServerRunner(uint16_t port, String origin = "", SensorsState sensorsState = SensorsState(), String protocol = "arduino");
    
    void loop();
};


#endif /* WEBSOCKETSSERVERCONFIG_H_ */
