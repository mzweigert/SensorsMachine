/*
 	Thread.h - An Thread object

	Thread is responsable for holding the "action" for something,
	also, it responds if it "should" or "should not" run, based on
	the current time;

	For instructions, go to https://github.com/ivanseidel/ArduinoThread

	Created by Ivan Seidel Gomes, March, 2013.
	Released into the public domain.
*/

#ifndef Thread_h
#define Thread_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <inttypes.h>

/*
	Uncomment this line to enable ThreadName Strings.

	It might be usefull if you are loging Thread with Serial,
	or displaying a list of Threads...
*/
// #define USE_Thread_NAMES	1
class Thread {
  private:
    typedef std::function<void(void)> _callbackType;
  protected:
    // Desired interval between runs
    unsigned long interval;

    // Last runned time in Ms
    unsigned long last_run;

    // Scheduled run in Ms (MUST BE CACHED)
    unsigned long _cached_next_run;

    /*
    	IMPORTANT! Run after all calls to run()
    	Updates last_run and cache next run.
    	NOTE: This MUST be called if extending
    	this class and implementing run() method
    */
    virtual void runned(long time);

    // Default is to mark it runned "now"
    virtual void runned() {
      runned(millis());
    }

    // Callback for run() if not implemented
    _callbackType _onRun;

  public:

    // If the current Thread is enabled or not
    bool enabled;


    // ID of the Thread (initialized from memory adr.)
    int ThreadID;

#ifdef USE_Thread_NAMES
    // Thread Name (used for better UI).
    String ThreadName;
#endif

    Thread(int _id, long _interval = 0, _callbackType _callback = NULL);

    // Set the desired interval for calls, and update _cached_next_run
    virtual void setInterval(long _interval);

    // Return if the Thread should be runned or not
    virtual bool shouldRun(long time);

    // Default is to check whether it should run "now"
    virtual bool shouldRun() {
      return shouldRun(millis());
    }

    // Callback set
    void onRun(_callbackType callback);
    // Runs Thread
    virtual void run();
};

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

#endif
