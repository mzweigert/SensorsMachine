#ifndef Thread_h
#define Thread_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

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
  virtual void runned() { runned(millis()); }

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
  void setInterval(long _interval);

  // Return if the Thread should be runned or not
  bool shouldRun(long time);

  // Default is to check whether it should run "now"
  bool shouldRun() { return shouldRun(millis()); }

  // Callback set
  void onRun(_callbackType callback);
  // Runs Thread
  virtual void run();

  void loop();
};

#endif
