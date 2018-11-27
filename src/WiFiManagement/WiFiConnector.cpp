#include "WiFiConnector.h"

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager


bool WiFiConnector::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

bool WiFiConnector::connectToWiFi() {

  WiFiManager wifiManager;

  return wifiManager.autoConnect("AutoConnectAP");

}
