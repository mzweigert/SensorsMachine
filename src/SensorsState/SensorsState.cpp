#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <inttypes.h>
#include "SensorsState.h"
#include "DHT.h"
#include "RCSwitch.h"

#define MAX_ANALOG_VAL 1024
#define DHT11_PIN 2
#define DHTTYPE DHT11
#define RECIVIER_DATA_PIN 0

DHT dht(DHT11_PIN, DHTTYPE);
RCSwitch receiver = RCSwitch();

SensorsState::SensorsState() {
  dht.begin();
  receiver.enableReceive(RECIVIER_DATA_PIN);
}

int SensorsState::readTemperature() {
  float temperature = dht.readTemperature();
  int repeated = 10;
  while (temperature != temperature) {
    temperature = dht.readTemperature();
    yield();
  }
  return temperature;
}

int SensorsState::readHumidity() {
  float humidity = dht.readHumidity();
  while (humidity != humidity) {
    humidity = dht.readHumidity();
    yield();
  }
  return humidity;
}

int SensorsState::readSoilMoisture() {
   int soilMoisture = 0;
   if (receiver.available()) {
    soilMoisture = receiver.getReceivedValue();
    receiver.resetAvailable();
    yield();
  }
  return soilMoisture;
}

String SensorsState::readAsJSON() {
  char message[100];
  int soilMoisture = readSoilMoisture();
  float temp = readTemperature();
  float hum = readHumidity();

  sprintf(message, "{ soil_moisture: \"%d\", temperature: \"%f\", humidity: \"%f\" }", soilMoisture, temp, hum);
  return (String)message;
}
