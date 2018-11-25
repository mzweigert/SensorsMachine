#ifndef SensorsState_h
#define SensorsState_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <inttypes.h>

#include "src/DHT/DHT.h"

#define MAX_ANALOG_VAL 1024
#define DHT11_PIN 2
#define DHTTYPE DHT11
DHT dht(DHT11_PIN, DHTTYPE);

class SensorsState {
  public:
    SensorsState() {
      dht.begin();
    }
    
    float readTemperature() {
      float temperature = dht.readTemperature();
      int repeated = 10;
      while (temperature != temperature && repeated > 0) {
        temperature = dht.readTemperature();
      }
      return temperature;
    }
    
    float readHumidity() {
      float humidity = dht.readHumidity();
      int repeated = 10;
      while (humidity != humidity && repeated > 0) {
        humidity = dht.readHumidity();
      }
      return humidity;
    }
    
    float readSoilMoisture() {
      int soilMoisture = ((analogRead(A0) * -1 + MAX_ANALOG_VAL) * 125) / MAX_ANALOG_VAL;
      return soilMoisture;
    }

    String readAsJSON() {
      char message[100];
      int soilMoisture = readSoilMoisture();
      float temp = readTemperature();
      float hum = readHumidity();

      sprintf(message, "{ soil_moisture: \"%d\", temperature: \"%f\", humidity: \"%f\" }", soilMoisture, temp, hum);
      return (String)message;
    }
};

#endif
