#include "ThreadController.h"

ThreadController::ThreadController(long _interval) : Thread((int)this) {
  cached_size = 0;

  clear();
  this->setInterval(_interval);

#ifdef USE_THREAD_NAMES
  // Overrides name
  ThreadName = "ThreadController ";
  ThreadName = ThreadName + ThreadID;
#endif
}

/*
  ThreadController run()
*/

void ThreadController::run() {
  if (this->_onRun != NULL) {
    this->_onRun();
  }

  long time = millis();
  int checks = 0;
  typename std::unordered_map<int, Thread *>::iterator it = threads.begin();

  int i = 0;
  // Iterate over the map using Iterator till end.
  while (it != threads.end() && i < MAX_THREADS && checks <= cached_size) {
    Thread *thread = it->second;

    if (thread != NULL) {
      checks++;
      i++;
      if (thread->shouldRun(time)) {
        thread->run();
      }
    }

    it++;
  }

  // ConsumerController extends Consumer, so we should flag as hasRun Thread
  this->runned();
}

bool ThreadController::add(Thread *_thread) {
  // Check if the Thread already exists on the array
  if (threads.find(_thread->ThreadID) != threads.end()) {
    return true;
  }

  if (threads.size() <= MAX_THREADS) {
    threads[_thread->ThreadID] = _thread;
    cached_size++;
    return true;
  }

  // Array is full
  return false;
}

void ThreadController::clear() {
  threads.clear();
  cached_size = 0;
}

int ThreadController::size(bool cached) {
  if (cached) return cached_size;

  cached_size = threads.size();

  return cached_size;
}

void ThreadController::stop(int _id) {
  if (threads.find(_id) == threads.end()) {
    return;
  }

  Thread *thread = threads.at(_id);

  thread->enabled = false;
  cached_size--;
  threads.erase(_id);
}