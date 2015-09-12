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

#ifndef FRSIMTHREADTRACKER_H
#define FRSIMTHREADTRACKER_H
#include <map>
#include <cstdlib>
extern "C" {
#include "pth.h"
}

class FrSimThread;
class FrSimThreadId;

/**
 * This singleton class is accessed from many different threads and keeps global
 * information about which threads are running and which ones are blocked.
 * It perhaps can be part of the SimThread class, and maybe I'll roll it back
 * into that class. The methods would be fine to be part of any SimThread object
 * and the few private data variables would have to be static. There would also
 * have to be a static method that initialized the static data, doing basically
 * what the constructor of the singleton would otherwise have to do.
 *
 * In addition to FrSimThread calling these methods, incRunning and decRunning 
 * would be called by any sync primitive wrappers that I make. If there are many
 * of these primitives does it just become better to keep the thread list in 
 * it's own (singleton) class?
 *
 * Maybe FrSimThreadTracker would be a better name?
 * Alt names: SimThreadMaster, SimThreadCtrl, SimThreadIface, 
 */
class FrSimThreadTracker {
 public:
  // to get access to the singleton. Constructors must be private.
  static FrSimThreadTracker* getThreadTracker();

  // All these calls must protect the common data with a mutex since they can be
  // called from any thread.
  // Really, they will normally only be called from the FrSimThread class methods.
  // add a newly created thread to the list and increment number of running and
  // total threads. Called by the SimThread Constructor.
  // In my new scheme, this would actually call pthread_create, update the
  // global data structures, and return a reference to the caller, which would 
  // always be the SimThread constructor.
  //int createThread(void *(*sr)(void*), void *srd); 
  void addThread(FrSimThread* t);
  // remove an existing thread from the list and decrement running and total
  // threads (unless thread removed when blocked). Called by SimThread Destrctor
  // In the new scheme, it would also call pthread_cancel.
  //void destroyThread(int refid);
  void delThread(FrSimThread* t);
  // In the new scheme, this is also necessary. Returns the thread's return code
  //int joinThread(int refid);

  // called by thread when it blocks it's self on an at() or a user sync prim
  // condition wait(). Signals the verilog callback to wake-up when 
  // runningThreads is decremented to zero. Therefore, this class must hold a
  // condition variable to do that signaling. The condition variable must be set
  // up when this class is constructed by the SimIface class.
  // The total number of runningThreads doesn't include the simulator thread
  // This is probably actually called by block() above, and maybe from the mutex
  // and condition classes.
  // Under the new scheme, this should block the calling thread--call with refid
  // just to make it easier. The name is actually wrong now because it also must
  // mark the appropriate thread as blocked and actually block in this call.
  //void decRunningThreads(int refid);
  void decRunningThreads();
  // called by verilog callback when wait condition for at() is met, or by user
  // sync prim when thread unblocked
  //void incRunningThreads(int refid);
  void incRunningThreads();
  // called by FrSimThread to support the getSelf call
  FrSimThread* getThread(FrSimThreadId *id);
  // called by the verilog callback, via SimThread.unblock() to wait until all
  // sim threads are blocked so the simulator can start again. It just waits on
  // the allThreadsBlocked condition.
  void waitAllThreadsBlocked();

  // debug support
  // These can only really be called safely when all threads are blocked. That's
  // the only time that the thread tracker is completely consistant with all the
  // simThread objects. DOes that indicate a problem with locking granularity?
  void printAllThreads();
  void printRunningThreads();
  void checkConsistancy();
 protected:

 private:
  // This class is a singleton so the constructor is private
  FrSimThreadTracker();
  FrSimThreadTracker(const FrSimThreadTracker& stt);
  FrSimThreadTracker& operator=(const FrSimThreadTracker& stt);
  static FrSimThreadTracker* stt;

  // Teal uses an array of threads set to true or false for running, but this
  // seems like it could be easier, because the simulator can go again when
  // running_threads is decremented to zero. These counts really just hold
  // summary information from the threadList, so a special function could be
  // written to check consistancy periodically while debugging the library.
  // The total thread count is probably not actually needed or used.
  int numRunningThreads;
  int numTotalThreads;
  int waitCount;
  // some sort of standard library container with a list of all simthread objects
  // it might be a map referenced by pthread_t and containing a bunch of SimThread
  // This could also just be an array, the pthread_t is already stored in the 
  // simThread object. It depends whether we actually have to find a simthread
  // obj by pthread_t, which I think we do have to for the getSelf call, which 
  // will be called fairly frequently.
  std::map<pth_t, FrSimThread*> threadList;

};

#endif



