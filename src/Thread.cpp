#include "Thread.h"

Thread::Thread(int _id, long _interval, _callbackType _callback) {
  enabled = true;
  onRun(_callback);
  _cached_next_run = 0;
  last_run = 0;

  ThreadID = _id;
#ifdef USE_Thread_NAMES
  ThreadName = "Thread ";
  ThreadName = ThreadName + ThreadID;
#endif

  setInterval(_interval);
}

void Thread::runned(long time) {
  // If less than 0, than get current ticks
  if (time < 0)
    time = millis();

  // Saves last_run
  last_run = time;

  // Cache next run
  _cached_next_run = last_run + interval;
}

void Thread::setInterval(long _interval) {
  // Filter intervals less than 0
  interval = (_interval < 0 ? 0 : _interval);

  // Cache the next run based on the last_run
  _cached_next_run = last_run + interval;
}

bool Thread::shouldRun(long time) {
  // If less than 0, then get current ticks
  if (time < 0)
    time = millis();

  // Exceeded the time limit, AND is enabled? Then should run...
  return ((time >= _cached_next_run) && enabled);
}

void Thread::onRun(_callbackType callback) {
  _onRun = callback;
}

void Thread::run() {
  if (_onRun != NULL)
    _onRun();

  // Update last_run and _cached_next_run
  runned();
}

void Thread::loop() {
  if (this->shouldRun()) {
    this->run();
  }
}