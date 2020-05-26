#ifndef SensorsState_h
#define SensorsState_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <I2CSoilMoistureSensor.h>
#include <Wire.h>
#include <inttypes.h>

#include <map>
#include <set>
#include <unordered_map>

#include "Definitions.h"

#define SENSOR_ADDRESS 0x20
#define MIN_SENSOR_CAPACITY 290
#define MAX_SENSOR_CAPACITY 530

struct I2C {
  short index;
  uint8_t sda, scl;

  // constructor
  I2C(short index, uint8_t sda, uint8_t scl) : index(index), sda(sda), scl(scl) {}

  // overload < operator to use a Node object as a key in a std::map
  // It returns true if the current object appears before the specified object
  bool operator<(const I2C &ob) const {
    return index < ob.index ||
           (index == ob.index && sda < ob.sda) ||
           (index == ob.index && sda == ob.sda && scl < ob.scl);
  }
};

class Sensor {
 private:
  short _id;
  I2C _i2c;
  I2CSoilMoistureSensor _sensor;

 public:
  Sensor(short _id, I2C _i2c);
  ~Sensor();
  short getId() {
    return _id;
  };

  I2C getI2C() {
    return _i2c;
  }

  byte readSoilMoisture();
  String readAsJSON();
};

class SensorsState {
 private:
  static byte _id_sequence;
  std::map<byte, Sensor *> _sensors;

  Sensor *findConnectedSensor(byte slot);
  bool isConnectedToSlot(byte slot);

 public:
  ~SensorsState();

  std::map<I2C, Sensor *> getConnections();

  String readAsJSON();
  signed char readSensorSoilMoisture(byte slot);
};

#endif