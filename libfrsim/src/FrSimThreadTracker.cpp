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

#include <cassert>
#include <iostream>
#include "FrSimThreadTracker.h"
#include "FrSimThread.h"

/******************************************************************************/
FrSimThreadTracker* FrSimThreadTracker::stt = 0;

/**
 * Constructor
 * This just does some initialization. There's no heap memory allocated, so the
 * destructor is kept trivial. The constructor is private because this class is
 * a singleton. There should never need to be more than one thread tracking 
 * instance for a process!
 */
FrSimThreadTracker::FrSimThreadTracker() {
  numRunningThreads = 0;
  numTotalThreads = 0;
}

/**
 * Returns the singleton instance of this class, creating it if it doesn't yet
 * exist.
 */
FrSimThreadTracker* FrSimThreadTracker::getThreadTracker() {
  if(stt==0) {
    stt = new FrSimThreadTracker();
  }
  return stt;
}

/******************************************************************************/

/**
 * This call actually starts the new thread and updates threadTracker data
 * structures for it. All access to the global threadTracker structures must be
 * protected by a mutex so it's in sync with which threads are started and all
 * it's data is consistant.
 */
void FrSimThreadTracker::addThread(FrSimThread* t) {
  t->startThread();
  numTotalThreads++;
  threadList[(t->getId())->id] = t;
  incRunningThreads();
}

/**
 * This call cancels the thread, unless it has already completed, and updates
 * the threadTracker data structures to represent the change.
 */
void FrSimThreadTracker::delThread(FrSimThread* t) {
  t->stopThread();
  numTotalThreads--;
  // FIXME: verify the thread exists before erasing it, just for error checking
  threadList.erase((t->getId())->id);
}

/**
 *
 */
void FrSimThreadTracker::decRunningThreads() {
  numRunningThreads--;
  // what if dec somehow is called before inc?
  assert(numRunningThreads>=0);
}

/**
 *
 */
void FrSimThreadTracker::incRunningThreads() {
  numRunningThreads++;
}

/**
 * This call is protected by the threadListMutex so that the data it returns
 * won't be partially updated. This removes the race condition between starting
 * a new thread an updating the thread list. The threadList update must complete
 * before this call returns, and this call must be used to access any thread
 * information. It also is called at the start of the wait() call.
 */
FrSimThread* FrSimThreadTracker::getThread(FrSimThreadId *th) {
  //  std::cout << " FrSimThreadTracker::getThread " << std::endl;
  FrSimThread* rval = NULL;
  std::map<pth_t, FrSimThread*>::iterator p = threadList.find(th->id);
  if(p!=threadList.end())
    rval = threadList[th->id];

  //  std::cout << " stt returning thread id : " << th->id << std::endl;
  return rval;
}

/**
 * 
 */
void FrSimThreadTracker::waitAllThreadsBlocked() {
  waitCount = 0;
  while (pth_ctrl(PTH_CTRL_GETTHREADS_READY | PTH_CTRL_GETTHREADS_NEW) != 0) {
  	waitCount++;
  	if (waitCount > 1500) {
  		std::cout << "FrSimThreadTracker thread spin detector!" << std::endl;
  		printRunningThreads();
  		std::exit(1);
  	}
//    std::cout << "Waiting for all threads : " << pth_ctrl(PTH_CTRL_GETTHREADS_READY | PTH_CTRL_GETTHREADS_NEW) << std::endl;
    pth_yield(NULL);
  }
}

/******************************************************************************/
// Debug routines

/**
 *
 */
void FrSimThreadTracker::printAllThreads() {
  // just iterate through the list of threads and print all their names and
  // status.
}

/**
 *
 */
void FrSimThreadTracker::printRunningThreads() {
  //  std::cout << " FrSimThreadTracker::getThread " << std::endl;
  std::map<pth_t, FrSimThread*>::iterator p = threadList.begin();
  pth_attr_t attr;
  pth_state_t state;

	do
	{
		attr = pth_attr_of(p->first);
		std::cout << p->second->threadName << "[" << p->second->getId()->id << "] ";
		pth_attr_get(attr, PTH_ATTR_STATE, &state);
		if (state == PTH_STATE_NEW) {
			std::cout << "NEW";
		} else if (state == PTH_STATE_READY) {
			std::cout << "READY";
		} else if (state == PTH_STATE_WAITING) {
			std::cout << "WAITING";
		} else {
			std::cout << "OTHER";
		}
		std::cout << std::endl;
		p++;
	} while (p != threadList.end());
	
}

/**
 *
 */
void FrSimThreadTracker::checkConsistancy() {
}




