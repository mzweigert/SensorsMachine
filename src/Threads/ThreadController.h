/*
 	ThreadController.h - Controlls a list of Threads with different timings
	Basicaly, what it does is to keep track of current Threads and run when
	necessary.
	ThreadController is an extended class of Thread, because of that,
	it allows you to add a ThreadController inside another ThreadController...
	For instructions, go to https://github.com/ivanseidel/ArduinoThread
	Created by Ivan Seidel Gomes, March, 2013.
	Released into the public domain.
*/

#ifndef ThreadController_h
#define ThreadController_h

#include "Thread.h"
#include "inttypes.h"
#include "../LinkedList/LinkedList.h"

#define MAX_THREADS		15


class ThreadController: public Thread{
protected:
	LinkedList<Thread*> threads = LinkedList<Thread*>();

	int cached_size;
public:
	ThreadController(long _interval = 0);

	// run() Method is overrided
	void run();

	// Adds a thread in the first available slot (remove first)
	// Returns if the Thread could be added or not
	bool add(Thread* _thread);

	// remove the thread (given the Thread* or ThreadID)
	void remove(int _id);
  void remove(Thread* _thread);

	// Removes all threads
	void clear();

	// Return the quantity of Threads
	int size(bool cached = true);

	// Return the I Thread on the array
	// Returns NULL if none found
	Thread* findById(int _id);

  void stop(int _id);
};


ThreadController::ThreadController(long _interval): Thread((int)this){
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

void ThreadController::run(){

   if(this->_onRun != NULL)
      this->_onRun();

  long time = millis();
  int checks = 0;
  for(int i = 0; i < MAX_THREADS && checks <= cached_size; i++){
    // Object exists? Is enabled? Timeout exceeded?
    
    Thread<uint8_t> *thread = threads.get(i);
    if(thread != NULL){
      checks++;
      if(thread->shouldRun(time)){
        thread->run();
      }
    }
  }

  // ThreadController extends Thread, so we should flag as hasRun thread
  this->runned();
}

/*
  List controller (boring part)
*/

bool ThreadController::add(Thread* _thread){
  // Check if the Thread already exists on the array
  Thread *thread = findById(_thread->ThreadID);
  if(thread != NULL){
    return true;
  }
  if(threads.size() <= MAX_THREADS) {
    threads.add(_thread);
    cached_size++;
    return true;
  }

  // Array is full
  return false;
}


void ThreadController::remove(int _id){
      Thread *thread = findById(_id);
      if(thread != NULL) {
          threads.remove(thread);
          cached_size--;
      }

    return;
}


void ThreadController::remove(Thread* _thread){
  remove(_thread->ThreadID);
}



void ThreadController::clear(){
  threads.clear();
  cached_size = 0;
}


int ThreadController::size(bool cached){
  if(cached)
    return cached_size;

  cached_size = threads.size();

  return cached_size;
}


Thread* ThreadController::findById(int _id){
    
    Thread *thread;
    for(int i = 0; i < threads.size(); i++){
      thread = threads.get(i);
      if(thread->ThreadID == _id) {
         return thread;
      }
    }
    return NULL;
}


void ThreadController::stop(int _id){
  Thread *thread;
  for(int i = 0; i < threads.size(); i++){
      thread = threads.get(i);
      if(thread != NULL && thread->ThreadID == _id) {
         interrupts();
         thread->enabled = false;
         threads.remove(i);
         noInterrupts();
      }
  }

}

#endif
