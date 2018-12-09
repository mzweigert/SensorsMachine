#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <inttypes.h>
#include "SensorsState.h"
#include "../DHT/DHT.h"

#define MAX_ANALOG_VAL 1024
#define DHT11_PIN 2
#define DHTTYPE DHT11
DHT dht(DHT11_PIN, DHTTYPE);

SensorsState::SensorsState() {
  dht.begin();
}

float SensorsState::readTemperature() {
  float temperature = dht.readTemperature();
  int repeated = 10;
  while (temperature != temperature) {
    temperature = dht.readTemperature();
     yield();
  }
  return temperature;
}

float SensorsState::readHumidity() {
  float humidity = dht.readHumidity();
  while (humidity != humidity) {
    humidity = dht.readHumidity();
    yield();
  }
  return humidity;
}

float SensorsState::readSoilMoisture() {
  int soilMoisture = ((analogRead(A0) * -1 + MAX_ANALOG_VAL) * 125) / MAX_ANALOG_VAL;
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
