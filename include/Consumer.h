#ifndef Consumer_h
#define Consumer_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <inttypes.h>

#include <vector>

/*
        Uncomment this line to enable ConsumerName Strings.

        It might be usefull if you are loging Consumer with Serial,
        or displaying a list of Consumers...
*/
// #define USE_Consumer_NAMES	1
template <typename Arg>
class Consumer {
 private:
  typedef std::function<void(Arg)> _callbackType;

 protected:
  // Desired interval between runs
  unsigned long interval;

  // Last runned time in Ms
  unsigned long last_run;

  // Scheduled run in Ms (MUST BE CACHED)
  unsigned long _cached_next_run;

  Arg _callbackArgument;
  /*
      IMPORTANT! Run after all calls to run()
      Updates last_run and cache next run.
      NOTE: This MUST be called if extending
      this class and implementing run() method
  */
  void runned(long time);

  // Default is to mark it runned "now"
  void runned() { runned(millis()); }

  // Callback for run() if not implemented
  _callbackType _onRun;

 public:
  // If the current Consumer is enabled or not
  bool enabled;

  // ID of the Consumer
  int ConsumerID;

#ifdef USE_Consumer_NAMES
  // Consumer Name (used for better UI).
  String ConsumerName;
#endif
  Consumer(int _id, long _interval = 0);
  Consumer(int _id, _callbackType _callback, Arg _arg, long _interval = 0);

  // Set the desired interval for calls, and update _cached_next_run
  void setInterval(long _interval);

  // Return if the Consumer should be runned or not
  bool shouldRun(long time);

  // Default is to check whether it should run "now"
  bool shouldRun() { return shouldRun(millis()); }

  // Callback set
  void onRun(_callbackType callback);
  // Runs Consumer
  void run();

  void loop();
};

template <typename Arg>
Consumer<Arg>::Consumer(int _id, long _interval) {
  enabled = true;
  _cached_next_run = 0;
  last_run = 0;

  ConsumerID = _id;
#ifdef USE_Consumer_NAMES
  ConsumerName = "Consumer ";
  ConsumerName = ConsumerName + ConsumerID;
#endif

  setInterval(_interval);
}

template <typename Arg>
Consumer<Arg>::Consumer(int _id, _callbackType _callback, Arg _arg,
                        long _interval) {
  enabled = true;
  _callbackArgument = _arg;
  onRun(_callback);
  _cached_next_run = 0;
  last_run = 0;

  ConsumerID = _id;
#ifdef USE_Consumer_NAMES
  ConsumerName = "Consumer ";
  ConsumerName = ConsumerName + ConsumerID;
#endif

  setInterval(_interval);
}

template <typename Arg>
void Consumer<Arg>::runned(long time) {
  // If less than 0, than get current ticks
  if (time < 0) time = millis();

  // Saves last_run
  last_run = time;

  // Cache next run
  _cached_next_run = last_run + interval;
}

template <typename Arg>
void Consumer<Arg>::setInterval(long _interval) {
  // Filter intervals less than 0
  interval = (_interval < 0 ? 0 : _interval);

  // Cache the next run based on the last_run
  _cached_next_run = last_run + interval;
}

template <typename Arg>
bool Consumer<Arg>::shouldRun(long time) {
  // If less than 0, then get current ticks
  if (time < 0) time = millis();

  // Exceeded the time limit, AND is enabled? Then should run...
  return ((time >= _cached_next_run) && enabled);
}

template <typename Arg>
void Consumer<Arg>::onRun(_callbackType callback) {
  _onRun = callback;
}

template <typename Arg>
void Consumer<Arg>::run() {

  if (_onRun != NULL) {
    _onRun(_callbackArgument);
  }

  // Update last_run and _cached_next_run
  runned();
}

template <typename Arg>
void Consumer<Arg>::loop() {
  if (this->shouldRun()) {
    this->run();
  }
}

#endif
