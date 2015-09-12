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

#include "FrSimIfaceIvl.h"
#include <iostream>
#include <cstdlib>

/******************************************************************************/

/**
 * Default constructor
 * Constructor is private since this is a singleton class
 * It currently does nothing, but I might move some of the stuff from the 
 * frsim_main_wrapper and startUserProgram to here to simplify things. Then the 
 * calltf function for the $frsim_system user task takes care of everything.
 */
FrSimIfaceIvl::FrSimIfaceIvl() {
}

/**
 * Copy Constructor
 * Constructor is declared private and defined as empty since this is a singleton
 * class.
 */
FrSimIfaceIvl::FrSimIfaceIvl(const FrSimIfaceIvl &sift) {}

/**
 * Assignment
 * Assignment operator is declared private and defined as empty since this is a
 * singleton class.
 */
void FrSimIfaceIvl::operator=(const FrSimIfaceIvl &sift) {}

/**
 * Destructor
 * It may be necessary to do something funky here since this is a singleton class
 * but I'm just leaving it empty for now.
 */
FrSimIfaceIvl::~FrSimIfaceIvl() {
}

/******************************************************************************/

/**
 * This is the main entry point to the class and the way it is usually accessed.
 * Returns a pointer to the single instance of this class.
 */
FrSimIfaceIvl* FrSimIfaceIvl::getSimIface() {
  if (sif == NULL) {
    sif = new FrSimIfaceIvl();
  }
  return dynamic_cast<FrSimIfaceIvl*>(sif);
}

/**
 * This call initializes the simulation threading system and starts the first
 * thread running the main test program.
 * This work is done inside the sif object so that the references to the 
 * simThreadTracker and the mainTestThread can be kept around. It might be a 
 * good idea to actually move this code into the constructor since it will only
 * be called once. Unless I decide to support restart at some point, but that
 * isn't there right now.
 */
void FrSimIfaceIvl::startUserProgram(void*(*testMain)(void *)) {
  stt = FrSimThreadTracker::getThreadTracker();
  mainTestThread = new FrSimThread("Main Test", testMain, 0);
  mainTestThread->spawn();
  stt->waitAllThreadsBlocked();
}

/******************************************************************************/

/**
 * Set a flag which causes the simulator to stop executing at the end of this
 * timestep.
 */
void FrSimIfaceIvl::stop() {
  vpi_control(vpiStop, 1);
}

/**
 * Set a flag which causes the simulator to stop executing at the end of this
 * timestep.
 * This can be called from any thread at any time so it must be threadsafe.
 */
void FrSimIfaceIvl::finish() {
  vpi_control(vpiFinish, 0);
}

/**
 * I'm considering having restart support, but it just causes the sim to finish
 * for now.
 */
void FrSimIfaceIvl::restart() {
  finish();
}

/**
 * Returns the current simulation time. 
 * Should probably return a type of FrSimTime or something similar.
 * There should be another version that just returns an int for simplicity.
 */
void FrSimIfaceIvl::getTime(FrSimTime* t) {
  vpiHandle h = 0;
  s_vpi_time curtime;
  curtime.type = vpiSimTime;
  vpi_get_time(h, &curtime);
  t->type = 0;
  t->high = curtime.high;
  t->low = curtime.low;
  t->real = 0;
}

/**
 * Returns the current simulation time. 
 * Only returns an int, so it can run out of space in a long simulation, but it
 * is very simple to use.
 */
int FrSimIfaceIvl::getTime() {
  vpiHandle h = 0;
  s_vpi_time curtime;
  curtime.type = vpiSimTime;
  vpi_get_time(h, &curtime);
  return curtime.low;
}


/******************************************************************************/

/**
 * Connect
 * Connects the FrSimReg to the RTL signal
 * Add a new entry to the siglist array which is the handle to this named signal
 * Store the array index for this entry in the FrSimReg so we can access it 
 * quickly and easily. This call is generally only made by the FrSimCReg 
 * constructor.
 *
 * FIXME: for output types, we need to verify that we're storing a handle to a 
 * reg. Driving a net won't work. For input types, we really would only normally
 * want a handle to a net, but a reg might be OK for certain monitor cases. For
 * a bidir we need a handle to a reg to drive and a net (probably) to sample. 
 * This also means that a simple list of vpiHandle isn't OK any longer and that
 * there must be a list of struct... or I could simply create two handles for
 * bidir signals and always drive the ref vpiHandle but sample ref+1.
 *
 * DEBUG: should I also keep a reference to the FrSimReg for debug purposes?
 * I should probably keep track of the direction of the connected signal.
 * May want to list other information about connected signals: width?
 */
void FrSimIfaceIvl::connect(FrSimCReg* r) {
  vpiHandle sigHandle = vpi_handle_by_name(r->getName(), NULL);
  if (sigHandle == NULL) { // could not connect handle
    printf("Could not connect to signal name %s in the verilog\n", r->getName());
    exit(1);
  }
  sigList.push_back(sigHandle);
  int ref = sigList.size() - 1;
  //PLI_INT32 vector_size = vpi_get(vpiSize, sigHandle);
  printf("FrSimIfaceIvl::connect: connected to signal %s, ref=%d type=%d\n", 
	 r->getName(), ref, vpi_get(vpiType, sigHandle));
  r->ref = ref;
}


/**
 * Disconnect
 * Disconnects FrSimReg from RTL signal. Probably just release the handle and 
 * remove or clear that entry from the conSigList array. This call is generally
 * only made by the FrSimCReg destructor.
 *
 * It's not a problem to just never disconnect and release handles for now. I
 * probably should set the handle to null and check for that in sample and drive
 * so sigs can't be accessed after disconnect. 
 */
void FrSimIfaceIvl::disconnect(FrSimCReg *r) {

}


/**
 * Sample
 * Samples the value for signal and updates the value stored in the FrSimCReg
 * type.
 * 
 * Should only allow sample of input and bidir type signals. The checks to make
 * sure the vpiHandle is appropriate to sample should have been made in the 
 * connect call. This call just needs to check that samples are allowed, which
 * might mean that out list of vpiHandle may need to become a list of some 
 * struct that holds the vpiHandle and other information.
 * Or, I guess the FrSimCReg could just hold the direction information.
 *
 * Since the FrSimRegVecVal has the same members and sizes as the verilog vecval
 * type, I can just cast it and use it to avoid an extra copy. This will
 * obviously break if either of those types have to change.
 */
void FrSimIfaceIvl::sample(const FrSimCReg* r) {
  s_vpi_value vpi_value_s; // FIXME: should I keep a single s_vpi_value around to avoid alloc/dealloc on every sample/drive?
  vpi_value_s.format = vpiVectorVal;
  vpiHandle sigHandle = sigList[r->ref];
  vpi_get_value(sigHandle, &vpi_value_s);

  //printf("Sampling signal %s got value aval=%d/bval=%d\n", r->getName(), 
  // vpi_value_s.value.vector[0].aval, vpi_value_s.value.vector[0].bval);
  r->setVecvalArray((FrSimRegVecVal*)vpi_value_s.value.vector);
}


/**
 * Drive
 * Drives the value for signal from the current value in the FrSimCReg object.
 *
 * Similar to sample, should only allow driving outputs and bidir signals. It
 * should check the registered direction and warn and refuse to perform drives
 * that aren't supported. 
 *
 * Again, since a FrSimRegVecVal has the same structure as a s_vpi_vecval, I
 * just cast and pass it in as the drive value. 
 */
void FrSimIfaceIvl::drive(const FrSimCReg* r) {
  s_vpi_value vpi_value_s; // FIXME: should I keep a single s_vpi_value around to avoid alloc/dealloc on every sample/drive?
  vpi_value_s.format = vpiVectorVal;
  vpiHandle sigHandle = sigList[r->ref];
  FrSimRegVecVal *sigVecval = r->getVecvalArray();
  //printf("Driving signal %s (%d)with value aval=%d/bval=%d\n", r->getName(), r->getRefId(),
  // sigVecval[0].aval, sigVecval[0].bval);

  vpi_value_s.value.vector = (p_vpi_vecval)sigVecval;
  vpi_put_value(sigHandle, &vpi_value_s, 0, vpiNoDelay); // FIXME: should I use something other that vpiNoDelay--don't I want non-blocking assigns?
}


/******************************************************************************/

/**
 * Wait
 * Registers callbacks and evaluates conditions provided. This call allows sync
 * between the libfrsim c++ code and the simulator.
 *
 * This function waits on a condition. That is, it will register callbacks for
 * all signals that the condition is sensitive to and then force the thread to 
 * sleep. When any of the sensitive signals change, the condition is evaluated
 * to see if it is true, and if it is the callbacks are unregisterd and the sim
 * thread is woken back up.
 *
 * In general, all threads will be waiting for this call to complete while the 
 * simulator is running. This call must be thread safe. The requirements are a
 * fair bit easier to meet for cooperative multithreading like pth as opposed to
 * pre-emptive multithreading like pthreads.
 */
void FrSimIfaceIvl::wait(FrSimCond c) {
  FrSimThread *thr = FrSimThread::getSelf();
  registerSimCallbacks(&c, thr);
  thr->wait();
}


/******************************************************************************/
// Protected Functions
/******************************************************************************/

/**
 * Register callbacks for all sensitive signals in the condition
 * This will be a class method, or it may become the wait method.
 */
void FrSimIfaceIvl::registerSimCallbacks(FrSimCond *c, FrSimThread *t) {
  // Create a FrSimWaitInfo struct from the condition and current thread
  // may want to add this struct to the list of waiting threads
  // DEBUG: It could also keep a reference to that class for debug purposes.
  FrSimWaitInfo *wi = new FrSimWaitInfo;
  wi->c = c;
  wi->t = t;

  // Get the list of FrSimRegs from the condition and call registerVpiCallback
  // for each FrSimReg passing the FrSimCondInfo. The registerVpiCallback call
  // returns a cbInfo struct. Those structs must be stored in a list in order to
  // do the unregistering. Could that list be part of the simCondInfo struct?
  for (std::vector<FrSimCReg *>::iterator it = c->sigList.begin();
       it != c->sigList.end();
       it++) {
    // for each reg in the list, register a vpi callback
    vpiHandle h = sigList[(*it)->ref];
    registerVpiCallback(h, wi);
  }


}

/**
 * Register or unregister a single callback for a single signal (?)
 * This will be a class method
 */
void FrSimIfaceIvl::registerVpiCallback(vpiHandle h, FrSimWaitInfo *wi) {
  // Register a condition check callback for a single handle with the provided
  // FrSimWaitInfo struct. This call needs the vpiHandle for each simReg.
  s_cb_data *cb_data_s = new s_cb_data;
  p_vpi_value value_s = new s_vpi_value;
  p_vpi_time time_s = new s_vpi_time;

  time_s->type = vpiSuppressTime;
  value_s->format = vpiSuppressVal;
  cb_data_s->user_data = (char *)wi;
  cb_data_s->reason = cbValueChange;
  cb_data_s->cb_rtn = vpi_condition_check_callback;
  cb_data_s->time = time_s;
  cb_data_s->value = value_s;
  cb_data_s->obj = h; // this needs to be the vpiHandle for the given FrSimReg

  //FrSimCBInfo *cbInf = new FrSimCBInfo; // FIXME: who deletes this structure? It took four calls to new to create it!!!

  //cbInf->cb = cb_data_s;
  //cbInf->hndl = vpi_register_cb(cb_data_s);
  wi->cbData.push_back(cb_data_s);
  wi->cbHndls.push_back(vpi_register_cb(cb_data_s));

  // FIXME: the cbInf struct should be stored in the FrSimWaitInfo struct!
  // Alternately, the cb handles and s_cb_data could be stored in separate vectors...
  //return(cbInf);

}


/******************************************************************************/
// Non-Class verilog support functions
/******************************************************************************/

// Called from the callback, or part of the callback
// Not a class method
void removeSimCallbacks(FrSimWaitInfo *wi) {
  // go through the list of registered callbacks in the simWaitInfo struct and
  // unregister each one of them. FrSimWaitInfo could even be it's own class
  // which includes this method.

  // FIXME: maybe this can't be protected since it's called from the callback
  // or maybe it really isn't an object method at all. Even if it isn't protected
  // it can't be called from the sif interface because it isn't in the parent
  // class... Maybe FrSimCondInfo should be it's own class

  // More likely, this can be part of the callback. It can unregister all
  // callbacks AND free the memory used by the structs.
  for (std::vector<vpiHandle>::iterator it = wi->cbHndls.begin();
       it != wi->cbHndls.end();
       it++) {
    // For each CBInfo struct in the list, remove the callback by calling vpi_remove_cb on it's hndl member
    vpi_remove_cb(*it);
    //      delete (*it)->cb;
    //      delete (*it);
  }
  //wi->cbHndls.clear();

}

void removeVpiCallback() {
  // remove a single callback by handle. Probably don't need this call since the
  // work is trivial.
}

/**
 * This is the callback that will be registered on all FrSimRegs when a FrSimCond
 * is being waited on. When any of the sensitive sigs change, the condition is
 * evaluated to see if it is true. If the condition is true, the callbacks for
 * this condition are unregistered and the sim thread that is waiting on this 
 * condition is woken back up.
 */
int vpi_condition_check_callback(s_cb_data *cb_data) {
  FrSimWaitInfo *wi = (FrSimWaitInfo*) cb_data->user_data;

  if (wi->c->test()) {
    // std::cout << "vpi_signal_callback :" << tf_gettime() << ": Triggering thread" << std::endl;   	
    wi->t->trigger();
    removeSimCallbacks(wi); // could move this code locally
  }

  wi->t->stt->waitAllThreadsBlocked(); // make sure all woken threads are finished before resuming simulation

  return(0);
}


/**
 * This function is called by the verilog system task. It essentially initializes
 * the world, first starting the threading system, then creating the sim iface
 * singleton object, then creating the thread tracker singleton, then starting
 * the user program in the first thread.
 *
 * Maybe this should just move into the frsim_connect_calltf function. I'm not
 * sure why there is any value is breaking it out.
 */
void frsim_main_wrapper() {
  if (!pth_init()) {
    printf("FrSimIface: can't init pth library in frsim_main_wrapper()\n");
    exit(1);
  }

  FrSimIfaceIvl* sif = FrSimIfaceIvl::getSimIface();
  sif->startUserProgram(frsim_main);
}

/**
 * The next three functions and variable declaration are for the $frsim_system 
 * verilog system task. The first one is called at sim startup and the second 
 * one is called during simulation when the $frsim_system task is executed. The
 * third one declares the system task and the vlog_startup_routines variable 
 * just allows the simulator to find the frsim_system_register call so that it 
 * can set everything up.
 */
int frsim_connect_compiletf(char *user_data) {
  //  std::cout << "in compile callback" << std::endl;
  return 0;
}

int frsim_connect_calltf(char *user_data) {
  //  std::cout << "in main  callback" << std::endl;
  frsim_main_wrapper();
  return(0);
}

void frsim_system_register()
{
  s_vpi_systf_data *tf_data;

  tf_data = new s_vpi_systf_data;

  tf_data->type = vpiSysTask;
  tf_data->tfname = "$frsim_system";
  tf_data->calltf = frsim_connect_calltf;
  tf_data->compiletf = frsim_connect_compiletf;
  tf_data->sizetf    = 0;
  tf_data->user_data = 0;
  vpi_register_systf(tf_data);
}

void (*vlog_startup_routines[])() = {
  frsim_system_register,
  0
};

