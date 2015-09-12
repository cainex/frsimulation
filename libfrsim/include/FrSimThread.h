/*****************************************************************************
* Copyright (c) 2015 by Daniel Grabowski.
*
* This file is part of FrSim.
* 
* FrSim is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* FrSim is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with FrSim.  If not, see <http://www.gnu.org/licenses/>.
*
* Created by: Daniel Grabowski, Andrew Worcester
*
*****************************************************************************/

#ifndef FRSIMTHREAD_H
#define FRSIMTHREAD_H

#include <stdio.h>
#include <string>
extern "C" {
#include "pth.h"
}

//class FrSimThreadTracker;

#include "FrSimThreadTracker.h"
//class FrSimCond;

/**
 * This class wraps the threading library primarily because the number of 
 * waiting and running tasks must be tracked exactly in order for the simulation
 * to be repeatable, but also to provide an object-oriented thread interface and
 * to contain some locks and state that must be kept for each thread. There will
 * be one instance of this class for every thread running in the simulation,
 * possibly with the exception of the initial thread where the simulator is 
 * running.
 *
 * This class could be written completely generically in terms of threading libs
 * and push all threading lib details into the SimThreadList (or something)
 * class. This has a certain appeal since it would be easier to replace the
 * thread library at that point without changing any user code. It might mean
 * extra levels of function calls in some places, but it might not be too bad.
 * I'm not really sure where the performance bottlenecks will be anyway.
 *
 * If it was done that way then the user could only use the SimThread and SimReg
 * classes and be completely unaware that the SimIface and SimThreadList classes
 * were taking care of things in the background. The SimIface and SimThreadList
 * would be set up by the simulator at the very start of the simulation and
 * pointers to them would be initialized in the SimReg and SimThread classes. It
 * would be totally invisible to the user if those underlying classes were
 * changed, and thus if the simulator or thread library and OS was changed.
 *
 * One question that comes up is whether the simThread and SimReg will just end 
 * up being hollow shell classes. And would that be wrong. I guess I just have
 * to start implementing and see how it looks.
 *
 * It should be possible, though maybe not the default, to create a thread 
 * without starting it. That would allow setting some parameters that must be
 * set when the new thread starts. This class will probably be extended to make
 * the FrSimTest and FrSimTestMain and FrSimTest* classes that will be used to 
 * write all the tests. You won't pass the function to run in the thread to the
 * extended versions. That function will be implemented in the class as the run
 * function. The constructors should have a flag to automatically start the run
 * function in the new thread or not. Run() may actually have to be wrapped by
 * another static function so that the new thread has access to the class vars.
 *
 * Generally, a thread is started by calling the constructor and canceled by
 * calling the destructor. Calling join or cancel explicitly would leave the 
 * thread around but not running, as would calling the constructor with the do
 * not start flag. It may or may not be safe to restart simThread objects in
 * this "exists but not running" state.
 */

class FrSimThreadId {
public:
 pth_t id;
 FrSimThreadId() : id(0) {}
 FrSimThreadId(pth_t _id) : id(_id) {}
};

class FrSimThreadCondVar
{
private:
	pth_mutex_t mutex;
	pth_cond_t cond;
	
	bool isLocked;
	
public:
	FrSimThreadCondVar();
	~FrSimThreadCondVar();
	
	void lock();
	void unlock();
	
	void signal(const bool &bcast);
	void wait();
};

class FrSimThreadEvent
{
 private:
  pth_message_t msg;

 public:
  FrSimThreadEvent();
  ~FrSimThreadEvent();

  void wait();
  void trigger();

  pth_event_t ev;
  pth_msgport_t msgPrt;

};

class FrSimThread {
 public:
  // Two forms for the constructor. First is passed nothing and the function to 
  // run in the thread must be set separately and then started. In the second,
  // the function and it's data are passed to the constructor and the thread is
  // started automatically.
  // Thread must be registerd with the global thread list.
  // Maybe there should be a default so that the thread is automatically named
  // if a name isn't given to the constructor. A default of "NoName" could be
  // keyed and automatically turned into a unique name like "UnnamedThread37"
  //FrSimThread(std::string name);
  FrSimThread(std::string name, void *(*sr)(void*), void *srd);
  // The destructor has a fair bit to do for this one:
  // What if the thread object is deleted when the thread is still running?
  // The destructor should make sure the thread is canceled properly.
  // Thread must be unregisted from the global thread list and the total number
  // of threads must be decremented.
  ~FrSimThread();

  // Get/Set attributes can only be set by the parent before the thread is
  //  started for some attributes. I.e. setStackSize, setSchedPolicy
  // Other get/set attributes can be set in the parent either before or after
  //  (to a limited extent) the thread is started. I.e. setDetachState, setSchedParam
  // Yet other get/set attributes are only meant to be set in the child. These
  //  include setCancelState

  // Probably the only extra attributes or functionality I need at first has to 
  // do with thread cancelation cleanup. If a thread is canceled while it is 
  // waiting for a callback, that callback should be removed. That could be done
  // in a couple of ways. The routine could be registered with pthreads, or it
  // could just be saved in the class which would call it after the thread
  // cancel. This stuff is important because threads that are canceled will
  // normally be canceled while waiting for a callback, so the callback needs to
  // be unregistered by something so it isn't called to unblock a dead thread!
  void setCleanupRoutine(void *(*cr)(void*), void *crd);
  void clearCleanupRoutine();

  // Methods that will only be called from other threads, often the parent, not
  // sure about return codes.

  void spawn();
  // Start actually calls pthread_create. It is called automatically from the
  // second form of the constructor, which is probably the normal case. The only
  // time this would be called separately is when thread attributes need to be
  // modified from the defaults, so it's probably a very rare case. This should
  // be set up to return an error if the thread is already started.
  // In extended versions of this class, start may actually call a run method
  // defined in the class.
  //void start(void *(*sr)(void*), void *srd);
  // Join waits for the thread to finish and returns the exit code. This would
  // return an exit code if the thread was already detached. This would be used
  // pretty often in verification. No effect if called by the thread in question
  int join();

  // For this to always work, I'll need to set up an FrSimThread object for the 
  // main simulator thread at startup. Maybe that should get created when 
  // FrSimThreadList is constructed.
  static FrSimThread* getSelf();
  static FrSimThreadId selfId;

  // Actually calls pthread_create. Separated out to ease sync with the thread
  // tracker and to allow future separation of create/start steps if desired.
  virtual int startThread();
  virtual int stopThread();

  // This is the function that is always passed to pthread_create. It basically
  // just runs the user function, but then automatically cleans up and
  // decrements the number of running threads when the user thread returns.
  // In FrSimTest, extended from this class, it will be overridden to run the
  // virtual run command within the class instead of startRoutine.
  static void* newThreadInit(void *data);

  FrSimThreadId* getId() { return id; }
  void yield();
  void wait();
  void trigger();

  // Generic information set per-thread
  std::string threadName;
  FrSimThreadTracker *stt; 

 protected:

  // The user routine and data that shall run in the thread. Probably not
  // necessary to keep around.
  void *(*startRoutine)(void*);
  void *startRoutineData;
  void *startRoutineReturn;
  // The cleanup routine to run should the thread get canceled
  void (*cleanupRoutine)(void*);
  void *cleanupRoutineData;

  FrSimThreadId *id;
  FrSimThreadEvent *ev;

  // GLOBAL THREAD TRACKING STUFF
  // maybe better to just keep the pointer to the threadlist singleton in each
  // class, rather then a static. I don't know if there is actually a difference
  // Every sim thread needs access to the overall list, and they'll all point to
  // the same one. Who sets the static variable? The constructor could set the
  // reference in each class if it was just a normal member.

};

template <class TClass> class FrSimThreadObj : public FrSimThread
{
 public:
  FrSimThreadObj(std::string name, TClass *srObj, void *(TClass::*sr)(void*), void *srd);
  virtual int startThread();
  
 private:
  void *(TClass::*startObjRoutine)(void*);
  TClass *startObj;
  static void *newObjThreadInit(void *data);

};

template <class TClass>
FrSimThreadObj<TClass>::FrSimThreadObj(std::string name, TClass *srObj, void *(TClass::*sr)(void*), void *srd) :
  FrSimThread(name, NULL, srd),
  startObjRoutine(sr),
  startObj(srObj)
{

}

template <class TClass>
int FrSimThreadObj<TClass>::startThread()
{

  printf("Obj start thread\n");
  fflush(stdout);

  // Should add attributes as class members
  pth_attr_t attr;
  attr = pth_attr_new();
  pth_attr_set(attr, PTH_ATTR_NAME, threadName.c_str());
  pth_attr_set(attr, PTH_ATTR_STACK_SIZE, 64*1024);
  pth_attr_set(attr, PTH_ATTR_JOINABLE, TRUE);

  pth_t tid = pth_spawn(attr, newObjThreadInit, (void *)this);
  
  if (tid == NULL) {
    return(1);
  } 
  
  FrSimThreadId *newId = new FrSimThreadId(tid);
  id = newId;
  
  return (0);

}

template <class TClass>
void *FrSimThreadObj<TClass>::newObjThreadInit(void *data)
{
  // Enumerate what has to be domeTh here:
  // - call stt->decRunningThreads() after the thread completes so we don't wait
  // for it!
   FrSimThreadObj<TClass>* meTh = (FrSimThreadObj<TClass> *)data; 
   (meTh->startRoutineReturn) = (*(meTh->startObj).*(meTh->startObjRoutine))(meTh->startRoutineData); 
   meTh->stt->decRunningThreads(); 
   return meTh->startRoutineReturn; 

}

#endif


