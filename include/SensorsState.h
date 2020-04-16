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

#include <vector>

#define TX 1
#define RX 3

#define SENSOR_ADDRESS 0x20
#define MIN_SENSOR_CAPACITY 290
#define MAX_SENSOR_CAPACITY 530

class Sensor {
 private:
  short _id;
  uint8_t *_sda, *_scl;
  I2CSoilMoistureSensor _sensor;

 public:
  Sensor(short _id, uint8_t *_sda, uint8_t *_scl);
  ~Sensor();
  short getId() {
    return _id;
  };

  String readData();
  String readAsJSON();
};

class SensorsState {
 private:
  static short _id_sequence;
  short _addresses_size;

 public:
  std::vector<Sensor> sensors = std::vector<Sensor>();

  SensorsState();
  ~SensorsState();

  Sensor *connectNext();
  bool isAllConnected();
  bool isAnyConnected();

  String readAsJSON();
};

#endif