#ifndef ConsumerController_h
#define ConsumerController_h

#include <unordered_map>

#include "Consumer.h"

#define MAX_consumers 15

template <typename Arg>
class ConsumerController : public Consumer<Arg> {
 protected:
  std::unordered_map<int, Consumer<Arg>*> consumers;

  int cached_size;

 public:
  ConsumerController(long _interval = 0);

  // run() Method is overrided
  void run();

  // Adds a Consumer in the first available slot (remove first)
  // Returns if the Consumer could be added or not
  bool add(Consumer<Arg>* _Consumer);

  // Removes all consumers
  void clear();

  // Return the quantity of consumers
  int size(bool cached = true);

  void stop(int _id);
};

template <typename Arg>
ConsumerController<Arg>::ConsumerController(long _interval)
    : Consumer<Arg>((int)this) {
  cached_size = 0;

  clear();
  this->setInterval(_interval);

#ifdef USE_Consumer_NAMES
  // Overrides name
  ConsumerName = "ConsumerController ";
  ConsumerName = ConsumerName + ConsumerID;
#endif
}

/*
  ConsumerController run()
*/
template <typename Arg>
void ConsumerController<Arg>::run() {
  if (this->_callbackArgument != NULL && this->_onRun != NULL) {
    this->_onRun(this->_callbackArgument);
  }

  long time = millis();
  int checks = 0;
  typename std::unordered_map<int, Consumer<Arg>*>::iterator it = consumers.begin();

  int i = 0;
  // Iterate over the map using Iterator till end.
  while (it != consumers.end() && i < MAX_consumers && checks <= cached_size) {
    Consumer<Arg>* consumer = it->second;

    if (consumer != NULL) {
      checks++;
      i++;
      if (consumer->shouldRun(time)) {
        consumer->run();
      }
    }

    it++;
  }

  // ConsumerController extends Consumer, so we should flag as hasRun Consumer
  this->runned();
}

/*
  List controller (boring part)
*/
template <typename Arg>
bool ConsumerController<Arg>::add(Consumer<Arg>* _consumer) {
  // Check if the Consumer already exists on the array
  if (consumers.find(_consumer->ConsumerID) != consumers.end()) {
    return true;
  }

  if (consumers.size() <= MAX_consumers) {
    consumers[_consumer->ConsumerID] = _consumer;
    cached_size++;
    return true;
  }

  // Array is full
  return false;
}

template <typename Arg>
void ConsumerController<Arg>::clear() {
  consumers.clear();
  cached_size = 0;
}

template <typename Arg>
int ConsumerController<Arg>::size(bool cached) {
  if (cached) return cached_size;

  cached_size = consumers.size();

  return cached_size;
}

template <typename Arg>
void ConsumerController<Arg>::stop(int _id) {
  if (consumers.find(_id) == consumers.end()) {
    return;
  }

  Consumer<Arg>* consumer = consumers.at(_id);
  consumer->enabled = false;
  cached_size--;
  consumers.erase(consumer->ConsumerID);
}

#endif
