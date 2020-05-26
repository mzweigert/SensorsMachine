#include "WaterPumpsController.h"

#define TURN_ON_PUMP 0x01
#define TURN_OFF_PUMP 0x00

#define PUMP_SDA D5
#define PUMP_SCL D6
#define PUMP_MCU_ADDRESS 0x08

WaterPumpsController::WaterPumpsController(SensorsState* sensorsState) {
  _sensorsState = sensorsState;
  _pumps = std::unordered_map<byte, WaterPump*>();
  for (byte i = 1; i <= 3; i++) {
    _pumps[i] = new WaterPump(i);
  }
}

void WaterPumpsController::sendCommand(byte number, byte powerCmd) {
  Serial.println((String) "Number: " + number + ", cmd: " + powerCmd);
  Serial.println((String) "Wire status: " + Wire.status());
  Wire.begin(PUMP_SDA, PUMP_SCL);
  Wire.beginTransmission(PUMP_MCU_ADDRESS);
  Wire.write(number);
  Wire.write(powerCmd);
  Wire.endTransmission();
}

void WaterPumpsController::turnOffPump(byte number) {
  _pumps[number]->state = WaterPumpState::WAITING;
  _pumps[number]->stopTime = millis();
  _pumps[number]->startTime = 0L;
  Serial.println((String) "Turn off pump : " + number);
  sendCommand(number, TURN_OFF_PUMP);
  sendCommand(number, TURN_OFF_PUMP);
}

void WaterPumpsController::changePumpStateToIdle(byte number) {
  WaterPump* pump = _pumps[number];
  pump->startTime = 0L;
  pump->stopTime = 0L;
  pump->state = WaterPumpState::IDLE;
  pump->timeToRun = 0L;
}

void WaterPumpsController::turnOnPump(byte number, int ms) {
  if (_pumps.find(number) == _pumps.end()) {
    return;
  }
  sendCommand(number, TURN_ON_PUMP);
  WaterPump* pump = _pumps[number];
  pump->startTime = millis();
  pump->stopTime = 0L;
  pump->state = WaterPumpState::RUNNED;
  pump->timeToRun = ms;
}

String WaterPumpsController::readAsJSON() {
  String json = "[";

  for (byte i = 1; i <= 3; i++) {
    json += _pumps[i]->readAsJSON();
    if (i != _pumps.size()) {
      json += ", ";
    }
  }

  json += "]";
  return json;
}

ProcessStatus WaterPumpsController::runWateringProcess(byte number) {
  if (_pumps.find(number) == _pumps.end()) {
    return PUMP_NOT_EXISTS;
  } else if (_pumps[number]->state != WaterPumpState::IDLE) {
    return PUMP_IS_WORKING;
  }

  signed char percent = _sensorsState->readSensorSoilMoisture(number);
  if (percent < 0) {
    return CORRESPONDING_SENSOR_IS_NOT_CONNECTED;
  } else if (percent >= 75) {
    return ALREADY_WATERED;
  } else {
    Serial.println((String) " Turn on pump: " + number);
    turnOnPump(number, RUN_TIME);
    return PUMP_HAS_RUNNED;
  }
}

void WaterPumpsController::loop() {
  for (byte i = 1; i <= 3; i++) {
    WaterPump* pump = _pumps[i];
    if (pump->state == WaterPumpState::RUNNED && millis() >= pump->startTime + pump->timeToRun) {
      Serial.println("Turn off pump: " + i);
      turnOffPump(i);
    } else if (pump->state == WaterPumpState::WAITING && millis() >= pump->stopTime + WAIT_TIME) {
      Serial.println(" Waiting pump: " + i);
      signed char percent = _sensorsState->readSensorSoilMoisture(i);

      if (percent >= 0 && percent < 75) {
        Serial.println(" Turn on pump: " + i);
        turnOnPump(i, RUN_TIME);
      } else {
        Serial.println(" Idle pump: " + i);
        changePumpStateToIdle(i);
      }
    }
  }
}