/*
 	ConsumerController.h - Controlls a list of consumers with different timings
	Basicaly, what it does is to keep track of current consumers and run when
	necessary.
	ConsumerController is an extended class of Consumer, because of that,
	it allows you to add a ConsumerController inside another ConsumerController...
	For instructions, go to https://github.com/ivanseidel/ArduinoConsumer
	Created by Ivan Seidel Gomes, March, 2013.
	Released into the public domain.
*/

#ifndef ConsumerController_h
#define ConsumerController_h

#include "Consumer.h"
#include "inttypes.h"
#include "../LinkedList/LinkedList.h"

#define MAX_consumers		15

template <typename Arg>
class ConsumerController: public Consumer<Arg> {
  protected:
    LinkedList<Consumer<Arg>*> consumers = LinkedList<Consumer<Arg>*>();

    int cached_size;
  public:
    ConsumerController(long _interval = 0);

    // run() Method is overrided
    void run();

    // Adds a Consumer in the first available slot (remove first)
    // Returns if the Consumer could be added or not
    bool add(Consumer<Arg>* _Consumer);

    // remove the Consumer (given the Consumer* or ConsumerID)
    void remove(int _id);
    void remove(Consumer<Arg>* _Consumer);

    // Removes all consumers
    void clear();

    // Return the quantity of consumers
    int size(bool cached = true);

    // Return the I Consumer on the array
    // Returns NULL if none found
    Consumer<Arg>* findById(int _id);

    void stop(int _id);
};

template <typename Arg>
ConsumerController<Arg>::ConsumerController(long _interval): Consumer<Arg>((int)this) {
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
  for (int i = 0; i < MAX_consumers && checks <= cached_size; i++) {
    // Object exists? Is enabled? Timeout exceeded?

    Consumer<uint8_t> *consumer = consumers.get(i);
    if (consumer != NULL) {
      checks++;
      if (consumer->shouldRun(time)) {
        consumer->run();
      }
    }
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
  Consumer<Arg> *consumer = findById(_consumer->ConsumerID);
  if (consumer != NULL) {
    return true;
  }
  if (consumers.size() <= MAX_consumers) {
    consumers.add(_consumer);
    cached_size++;
    return true;
  }

  // Array is full
  return false;
}

template <typename Arg>
void ConsumerController<Arg>::remove(int _id) {
  Consumer<Arg> *consumer = findById(_id);
  if (consumer != NULL) {
    consumers.remove(consumer);
    cached_size--;
  }

  return;
}

template <typename Arg>
void ConsumerController<Arg>::remove(Consumer<Arg>* _consumer) {
  remove(_consumer->ConsumerID);
}


template <typename Arg>
void ConsumerController<Arg>::clear() {
  consumers.clear();
  cached_size = 0;
}

template <typename Arg>
int ConsumerController<Arg>::size(bool cached) {
  if (cached)
    return cached_size;

  cached_size = consumers.size();

  return cached_size;
}

template <typename Arg>
Consumer<Arg>* ConsumerController<Arg>::findById(int _id) {

  Consumer<Arg> *consumer;
  for (int i = 0; i < consumers.size(); i++) {
    consumer = consumers.get(i);
    if (consumer->ConsumerID == _id) {
      return consumer;
    }
  }
  return NULL;
}

template <typename Arg>
void ConsumerController<Arg>::stop(int _id) {
  Consumer<Arg> *consumer;
  for (int i = 0; i < consumers.size(); i++) {
    consumer = consumers.get(i);
    if (consumer != NULL && consumer->ConsumerID == _id) {
      interrupts();
      consumer->enabled = false;
      consumers.remove(i);
      noInterrupts();
    }
  }

}

#endif