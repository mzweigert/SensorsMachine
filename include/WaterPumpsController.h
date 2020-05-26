#ifndef WaterPumps_h
#define WaterPumps_h

#include <ESP8266WiFi.h>
#include <Wire.h>

#include "Definitions.h"
#include "SH1106.h"
#include "SensorsState.h"
#include "ThreadController.h"

#define WAIT_TIME 10000
#define RUN_TIME 5000

enum ProcessStatus {
  PUMP_NOT_EXISTS,
  CORRESPONDING_SENSOR_IS_NOT_CONNECTED,
  PUMP_IS_WORKING,
  PUMP_HAS_RUNNED,
  ALREADY_WATERED
};

class WaterPumpState {
 public:
  enum Value : uint8_t {
    IDLE,
    RUNNED,
    WAITING
  };

  WaterPumpState() = default;
  constexpr WaterPumpState(Value state) : value(state) {}

  /*#if Enable switch (fruit) use case:
  operator Value() const { return value; }  // Allow switch and comparisons.
                                            // note: Putting constexpr here causes
                                            // clang to stop warning on incomplete
                                            // case handling.
  explicit operator bool() = delete;        // Prevent usage: if(fruit)
  #else*/
  constexpr bool operator==(WaterPumpState a) const { return value == a.value; }
  constexpr bool operator!=(WaterPumpState a) const { return value != a.value; }
  //#endif

  String name() {
    switch (value) {
      case IDLE:
        return "IDLE";
      case RUNNED:
        return "RUNNED";
      case WAITING:
        return "WAITING";
      default:
        return "UNKNOWN";
    }
  }

 private:
  Value value;
};

struct WaterPump {
  byte number;
  WaterPumpState state;
  unsigned long startTime;
  unsigned long stopTime;
  int timeToRun;
  WaterPump(byte number) : number(number) {
    state = WaterPumpState::IDLE,
    startTime = 0;
    stopTime = 0;
    timeToRun = 0;
  };
  String readAsJSON() {
    return (String) "{ \"number\" : " + number + ", \"state\" : \"" + state.name() + "\" }";
  }
};

class WaterPumpsController {
 private:
  std::unordered_map<byte, WaterPump*> _pumps;
  SensorsState* _sensorsState;

  void sendCommand(byte number, byte powerCmd);
  void turnOnPump(byte number, int ms);
  void turnOffPump(byte number);
  void changePumpStateToIdle(byte number);

 public:
  String readAsJSON();
  ProcessStatus runWateringProcess(byte number);
  void loop();

  WaterPumpsController(SensorsState* sensorsState);
};

#endif
