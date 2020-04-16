#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include "SensorsState.h"

uint8_t sensorPins[][3] = {
    {D7, D3, 0},
    {D2, D1, 0},
    {TX, RX, 0},
    {D5, D6, 0}};

void changePinsFunction(int mode, uint8_t* sda, uint8_t* scl) {
  if (*sda == TX && *scl == RX) {
    pinMode(*sda, mode);
    pinMode(*scl, mode);
    delay(100);
  }
}

short SensorsState::_id_sequence = 1;

SensorsState::SensorsState() {
  this->_addresses_size = sizeof(sensorPins) / (sizeof(uint8_t) * 3);
}

Sensor* SensorsState::connectNext() {

  for (int i = 0; i < _addresses_size; i++) {
    if (sensorPins[i][2] == 1) {
      continue;
    }

    Serial.println((String) "Trying to connect to pins : " + sensorPins[i][0] + " " + sensorPins[i][1]);
    Wire.begin(sensorPins[i][0], sensorPins[i][1]);
    Wire.setClockStretchLimit(2500);
    changePinsFunction(FUNCTION_3, &sensorPins[i][0], &sensorPins[i][1]);
    Wire.beginTransmission(SENSOR_ADDRESS);
    int error = Wire.endTransmission();
    if (error == 0) {

      Sensor sensor = Sensor(SensorsState::_id_sequence, &sensorPins[i][0], &sensorPins[i][1]);
      sensors.push_back(sensor);
      SensorsState::_id_sequence += 1;
      sensorPins[i][2] = 1;

      changePinsFunction(FUNCTION_0, &sensorPins[i][0], &sensorPins[i][1]);

      Serial.print("I2C device found at address 0x");
      if (SENSOR_ADDRESS < 16)
        Serial.print("0");
      Serial.print(SENSOR_ADDRESS, HEX);
      Serial.println("  !");

      return &sensors.back();
    } else {
      changePinsFunction(FUNCTION_0, &sensorPins[i][0], &sensorPins[i][1]);

      Serial.print("Unknown error at address 0x");
      if (SENSOR_ADDRESS < 16)
        Serial.print("0");
      Serial.print(SENSOR_ADDRESS, HEX);
      Serial.print(" with code: ");
      Serial.println(error);
    }
  }
  return NULL;
}

bool SensorsState::isAllConnected() {
  return this->sensors.size() == _addresses_size;
}

bool SensorsState::isAnyConnected() {
  return this->sensors.size() > 0;
}

SensorsState::~SensorsState() {
  this->sensors.clear();
}

String SensorsState::readAsJSON() {
  String json = "[";
  for (int i = 0; i < sensors.size(); i++) {
    String sensorData = sensors.at(i).readAsJSON();
    json += sensorData;
    if (i < sensors.size() - 1) {
      json += ", ";
    }
  }
  json += "]";
  return json;
}

Sensor::Sensor(short _id, uint8_t* _sda, uint8_t* _scl) : _id(_id), _sda(_sda), _scl(_scl) {
  this->_sensor = I2CSoilMoistureSensor();
  this->_sensor.begin();
}

Sensor::~Sensor() {}

String Sensor::readData() {
  changePinsFunction(FUNCTION_3, this->_sda, this->_scl);
  Wire.begin(*(this->_sda), *(this->_scl));
  Wire.setClockStretchLimit(2500);
  while (this->_sensor.isBusy()) delay(100);
  unsigned int capacitance = this->_sensor.getCapacitance();
  //this->_sensor.sleep();
  changePinsFunction(FUNCTION_0, this->_sda, this->_scl);

  delay(500);
  Serial.print("Sensor id: ");
  Serial.print(this->_id);
  Serial.print(", capacitance: ");
  Serial.print(capacitance);

  if (MAX_SENSOR_CAPACITY < capacitance)
    capacitance = MAX_SENSOR_CAPACITY;
  else if (MIN_SENSOR_CAPACITY > capacitance)
    capacitance = MIN_SENSOR_CAPACITY;

  int percent = ((capacitance - MIN_SENSOR_CAPACITY) * 100) / (MAX_SENSOR_CAPACITY - MIN_SENSOR_CAPACITY);
  return String(percent);
}

String Sensor::readAsJSON() {
  return "{ \"id\": " + String(this->_id) + ", \"soil_moisture\" : " + this->readData() + " }";
}