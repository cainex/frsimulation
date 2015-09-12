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

#include <iostream>
#include <cstdlib>
//#include "FrSimCond.h"
#include "FrSimThread.h"
//#include "FrSimThreadTracker.h"
	
FrSimThreadCondVar::FrSimThreadCondVar() :
	isLocked(false)
{
	pth_cond_init(&cond);
	pth_mutex_init(&mutex);
}

FrSimThreadCondVar::~FrSimThreadCondVar()
{
}

void FrSimThreadCondVar::lock()
{
	pth_mutex_acquire(&mutex, false, NULL);
	isLocked = true;
}

void FrSimThreadCondVar::unlock()
{
	pth_mutex_release(&mutex);
	isLocked = false;
}
	
void FrSimThreadCondVar::signal(const bool &bcast)
{
	pth_cond_notify(&cond, bcast);
}

void FrSimThreadCondVar::wait()
{
	pth_cond_await(&cond, &mutex, NULL);
}

FrSimThreadEvent::FrSimThreadEvent()
{
  msgPrt = pth_msgport_create(NULL);
  ev = pth_event(PTH_EVENT_MSG, msgPrt);
}

FrSimThreadEvent::~FrSimThreadEvent()
{
}

void FrSimThreadEvent::wait()
{
  pth_wait(ev);
  
//  pth_status_t status = pth_event_status(ev);
//  std::cout << "wait returned ";
//  if (status == PTH_STATUS_PENDING) {
//  	std::cout << "PENDING" << std::endl;
//  } else if (status == PTH_STATUS_OCCURRED) {
//  	std::cout << "OCCURED" << std::endl;
//  } else if (status == PTH_STATUS_FAILED) {
//  	std::cout << "FAILED" << std::endl;
//  }
  
//  std::cout << "waiting for " << pth_msgport_pending(msgPrt) << std::endl;
  int i = 0;
  while (pth_msgport_pending(msgPrt) != 0) {
  	pth_msgport_get(msgPrt);
//  	std::cout << "now waiting for " << pth_msgport_pending(msgPrt) << std::endl;
  	i++;
  	if (i == 10)
  		std::exit(1);
  }
}

void FrSimThreadEvent::trigger()
{
//  std::cout << "trigger before " << pth_msgport_pending(msgPrt) << std::endl;
  if (!pth_msgport_put(msgPrt, &msg)) {
    // need to put proper failure method here.
    exit(1);
  }
//  std::cout << "trigger after " << pth_msgport_pending(msgPrt) << std::endl;

  pth_yield(NULL);
}

/******************************************************************************/

/**
 * Constructor
 */
 FrSimThread::FrSimThread(std::string name, void *(*sr)(void*), void *srd) :
  threadName(name),
  startRoutine(sr),
  startRoutineData(srd),
  ev(new FrSimThreadEvent()) 

{
  stt = FrSimThreadTracker::getThreadTracker();
  // The addThread function actually calls pthread_create to start the thread
}

/**
 * Destructor
 */
 FrSimThread::~FrSimThread() {
  // Do I need to explicitly detect somebody callind delete on their own thread?
  // tell the thread tracker that the thread is done, it will cancel it
  stt->delThread(this);
  delete id;
}

/******************************************************************************/

/**
 * This routine is called when the thread is canceled. It would be used to clear
 * conditions that the simulator is waiting on for this thread.
 * This may not actually be necessary because the cancel called by another
 * thread should no be applied if we're blocked waiting for a mutex, which is
 * how the block call works. Maybe I can just call pthread_testcancel() before
 * I register a condition for any wait. If I do that, though, won't it be
 * possible to have lots of threads waiting around for callbacks that will never
 * come? I mean if I am waiting for a condition that will never occur, and I
 * cancel it, it won't really every cancel if I use deferred cancelation and 
 * only call testcancel before the next wait. Of course I normally don't code
 * that way, setting up lots of expect threads and canceling the ones that don't
 * work out, but it is a valid technique. Maybe I can fix it in the future.
 *
 * Current testbench code doesn't cancel threads at all. Ever. So I probably
 * shouldn't sweat it too much.
 */

FrSimThreadId FrSimThread::selfId;

void FrSimThread::setCleanupRoutine(void *(*cr)(void*), void *crd) {

}

/**
 * This function removes any routines registered with setCleanupRoutine above.
 */
 
void FrSimThread::clearCleanupRoutine() {

}

/**
 * This function adds the thread to the thread tracker list, which in turn
 * calls startThread
 */
 
void FrSimThread::spawn() {
  stt->addThread(this);
}

/**
 * FIXME: this currently assumes that the void* returned from the thread is 
 * actually an int*, which may not always be right.
 */
 
int FrSimThread::join() {
  pth_join(id->id, NULL);
  return 0;
}

/**
 * I guess this doesn't really have to be part of the class at this point. It
 * can just be it's own separate function. It depends on nothing from this class
 */
 
FrSimThread* FrSimThread::getSelf() {
  //  std::cout << "FrSimThread::getSelf" << std::endl;
  selfId.id = pth_self();
  return FrSimThreadTracker::getThreadTracker()->getThread(&selfId);
}

/**
 * This is a trivial function, but it is separated out so that it can be called
 * from the threadTracker which makes synchronization of it's data easier at
 * startup. I also may want to break out construction and start in the future.
 */
 
int FrSimThread::startThread() {

  printf("Base StartThread\n");
  fflush(stdout);

  // Should add attributes as class members
  pth_attr_t attr;
  attr = pth_attr_new();
  pth_attr_set(attr, PTH_ATTR_NAME, threadName.c_str());
  pth_attr_set(attr, PTH_ATTR_STACK_SIZE, 2048*1024);
  pth_attr_set(attr, PTH_ATTR_JOINABLE, TRUE);

  pth_t tid = pth_spawn(attr, newThreadInit, (void *)this);
  
  if (tid == NULL) {
    return(1);
  } 
  
  FrSimThreadId *newId = new FrSimThreadId(tid);
  id = newId;
  
  return (0);
}

/**
 * Stop could be called in a few situations
 * Right now it is only called from the destructor.
 * 1. The thread has already run to completion, possibly by calling join()
 * 2. The thread is still running, probably in the blocked state
 * 3. (not yet possible) thread created, but not yet started
 * 4. (not yet possible) thread created, started, and canceled explicily
 *
 * Perhaps the simThread class should try to keep explicit state. Already I set
 * a flag when a thread is blocked by calling block. I might want other state so
 * I can track if we're waiting on a simMutex or simCondition.
 *
 * Currently: If a thread runs to completion, newThreadInit will call 
 *  decRunningThreads to keep things consistant
 * If a thread is canceled without completing, nobody runs decRunningThreads
 * FIXME: Make sure the thread count gets updated no matter what!
 *
 * FIXME: RACE! Need some sort of barrier so that either the thread finishes by
 * its self, or it is canceled, but it needs to be clear which one is done
 * because decRunningThreads needs to be called once and only once at the end of
 * the thread's life.
 */
 
int FrSimThread::stopThread() {
  return pth_cancel(id->id);
}

/**
 * I thought that it would be necessary to wrap user functions so I would call
 * pthread_create on this function and have this function call the user function
 * after doing some startup stuff and do some cleanup stuff after. For now, at
 * least, it doens't seem necessary, but I'm leaving it here as a reminder of 
 * what I might have to do if I add some more flexibility.
 */
 
void* FrSimThread::newThreadInit(void *data) {
  // Enumerate what has to be done here:
  // - call stt->decRunningThreads() after the thread completes so we don't wait
  // for it!
  FrSimThread* me = (FrSimThread *)data;
  (me->startRoutineReturn) = (me->startRoutine)(me->startRoutineData);
  me->stt->decRunningThreads();
  return me->startRoutineReturn;
}

void FrSimThread::wait() {
  ev->wait();
}

//void FrSimThread::wait(FrSimThreadEvent *e) {
//	e->wait();
//}

void FrSimThread::trigger() {
  ev->trigger();
}

void FrSimThread::yield() {
	pth_yield(NULL);
}

