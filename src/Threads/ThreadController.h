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


#endif
