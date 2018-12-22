#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>

#include "WiFiConnector.h"
#include "SPIFFSReadServer.h"
#include "PersWiFiManager.h"

#define DEBUG_PRINT(x) Serial.println(x)
#define DEVICE_NAME "ESP8266 DEVICE"

SPIFFSReadServer server(80);
DNSServer dnsServer;
PersWiFiManager persWM(server, dnsServer);

bool WiFiConnector::isConnected() {
  return WiFi.isConnected();
}

bool WiFiConnector::connectToWiFi() {

  persWM.onConnect([]() {
    DEBUG_PRINT("wifi connected");
    DEBUG_PRINT(WiFi.localIP());
  });
  //...or AP mode is started
  persWM.onAp([]() {
    DEBUG_PRINT("AP MODE");
    DEBUG_PRINT(persWM.getApSsid());
  });


  SPIFFS.begin();
  //sets network name for AP mode
  persWM.setApCredentials(DEVICE_NAME);

  //make connecting/disconnecting non-blocking
  persWM.setConnectNonBlock(true);

  server.begin();
  return persWM.begin();
}

void WiFiConnector::loop() {
  //in non-blocking mode, handleWiFi must be called in the main loop
  persWM.handleWiFi();

  dnsServer.processNextRequest();
  server.handleClient();
}
