#include "WiFiConnector.h"
#include <ESP8266WiFi.h>

const char* ssid = "UPC87275D2";
const char* password = "zBpew5zpGumj";

bool WiFiConnector::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

bool WiFiConnector::connectToWiFi() {

  WiFi.begin(ssid, password);

  Serial.print("connecting");

  int triesNumber = 0;

  while (!isConnected()) {
    if (triesNumber >= 10) {
      Serial.println("Connection failed...");
      return false;
    }
    Serial.print(".");
    delay(500);
    triesNumber++;
  }

  Serial.println(WiFi.localIP());
  Serial.println("connected...");
  return true;
}
