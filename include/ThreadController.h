#ifndef ThreadController_h
#define ThreadController_h

#include <unordered_map>

#include "Thread.h"

#define MAX_THREADS 15

class ThreadController : public Thread {
 protected:
  std::unordered_map<int, Thread*> threads;

  int cached_size;

 public:
  ThreadController(long _interval = 0);

  // run() Method is overrided
  void run();

  // Adds a thread in the first available slot (remove first)
  // Returns if the Thread could be added or not
  bool add(Thread* _thread);

  // Removes all threads
  void clear();

  // Return the quantity of Threads
  int size(bool cached = true);

  void stop(int _id);
};

#endif