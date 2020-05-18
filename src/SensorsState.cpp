#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include "SensorsState.h"

uint8 sensorPins[][2] = {
    {D2, D1},   // LEFT UP
    {D5, D6},   // LEFT DOWN
    {D7, D3},   // RIGHT UP
    {TX, RX}};  // RIGHT DOWN

void changePinsFunction(int mode, uint8_t* sda, uint8_t* scl) {
  if (*sda == TX && *scl == RX) {
    pinMode(*sda, mode);
    pinMode(*scl, mode);
    delay(100);
  }
}

SensorsState::SensorsState() {
  this->_addresses_size = sizeof(sensorPins) / (sizeof(uint8_t) * 2);
}

SensorsState::~SensorsState() {
  this->_sensors.clear();
}

std::map<I2C, Sensor*> SensorsState::getConnections() {
  int i;
  std::map<I2C, Sensor*> newConnections = std::map<I2C, Sensor*>();
  for (i = 0; i < this->_addresses_size; i++) {
    Sensor* sensor = findConnectedSensor(i);
    if (sensor == NULL) {
      I2C i2c = I2C(i, sensorPins[i][0], sensorPins[i][1]);
      newConnections[i2c] = sensor;
    } else {
      newConnections[sensor->getI2C()] = sensor;
    }
  }
  return newConnections;
}

Sensor* SensorsState::findConnectedSensor(byte slot) {
  bool connected = isConnectedToSlot(slot);
  if (connected) {
    if (_sensors.find(slot) == _sensors.end()) {
      I2C i2c = I2C(slot, sensorPins[slot][0], sensorPins[slot][1]);
      Sensor* sensor = new Sensor(slot + 1, i2c);
      _sensors[slot] = sensor;
      delay(150);
    }
    return _sensors[slot];
  } else {
    if (_sensors.find(slot) != _sensors.end()) {
      _sensors.erase(slot);
    }
    return NULL;
  }
}

bool SensorsState::isConnectedToSlot(byte slot) {
  Wire.begin(sensorPins[slot][0], sensorPins[slot][1]);
  Wire.setClockStretchLimit(2500);
  changePinsFunction(FUNCTION_3, &sensorPins[slot][0], &sensorPins[slot][1]);
  Wire.beginTransmission(SENSOR_ADDRESS);
  int error = Wire.endTransmission();
  if (error == 0) {

    changePinsFunction(FUNCTION_0, &sensorPins[slot][0], &sensorPins[slot][1]);

    return true;
  } else {
    changePinsFunction(FUNCTION_0, &sensorPins[slot][0], &sensorPins[slot][1]);
    return false;
  }
}

String SensorsState::readAsJSON() {
  String json = "[";
  int i = 0;

  for (auto const& _sensor : _sensors) {
    Sensor* sensor = _sensor.second;
    json += sensor->readAsJSON();
    if (i != _sensors.size() - 1) {
      json += ", ";
    }
    i++;
  }

  json += "]";
  return json;
}

Sensor::Sensor(short _id, I2C _i2c) : _id(_id), _i2c(_i2c) {
  this->_sensor = I2CSoilMoistureSensor();
  this->_sensor.begin();
}

Sensor::~Sensor() {}

String Sensor::readData() {
  changePinsFunction(FUNCTION_3, &this->_i2c.sda, &this->_i2c.scl);
  Wire.begin(this->_i2c.sda, this->_i2c.scl);
  Wire.setClockStretchLimit(2500);
  while (this->_sensor.isBusy()) delay(50);
  unsigned int capacitance = this->_sensor.getCapacitance();
  //this->_sensor.sleep();
  changePinsFunction(FUNCTION_0, &this->_i2c.sda, &this->_i2c.scl);

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