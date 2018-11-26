#include "ThreadController.h"

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
    
    Thread *thread = threads.get(i);
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
          threads.remove(thread->ThreadID);
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