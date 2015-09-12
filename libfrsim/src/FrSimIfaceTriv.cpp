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

#include <stdio.h> // for printf

#include "FrSimIfaceTriv.h"
#include "FrSimThread.h"
#include "FrSimThreadTracker.h"


/******************************************************************************/
// FrTrivSimSig helper class

FrTrivSimSig::FrTrivSimSig(int w, char* name) {
  connected = false;
  width = w;
  // alloc vecval array of proper width; width is give in words, not bits
  curval = new FrSimRegVecVal[w];
  nxtval = new FrSimRegVecVal[w];
  // maybe save name for debug
}

int FrTrivSimSig::connect() {
  if(connected) {
    printf("FrSimIfaceTriv: multiple drivers not allowed on one signal!\n");
    exit(1);
  }
  return 1;
}
void FrTrivSimSig::drive(FrSimRegVecVal* val, int id) { // ID will eventually support multiple drivers. ignored for now.
  // copy val to nxtval
  for(int i=0; i<width; i++) {
    nxtval[i].aval = val[i].aval;
    nxtval[i].bval = val[i].bval;
  }
}

void FrTrivSimSig::update() {
  // copy nxtval to curval
  for(int i=0; i<width; i++) {
    curval[i].aval = nxtval[i].aval;
    curval[i].bval = nxtval[i].bval;
  }
}


  // eventually need curval and nxtval for every driver and a resolver function,
  // but I'm limiting to one driver per sig for now so things are simpler
  //FrSimRegVecVal* curval;
  //FrSimRegVecVal* nxtval;

/******************************************************************************/

/**
 * Default constructor
 * Constructor is private since this is a singleton class.
 */
FrSimIfaceTriv::FrSimIfaceTriv() {
  // What, if anything, special does the constructor have to do?
  // Initialize simulation time to zero
  // Perhaps initialize signal list and other simulation data structures.
  // Probably add the first signal, named CLOCK
  simDoneFlag = 0;
}

/**
 * Copy Constructor
 * Constructor is private since this is a singleton class. It should never be
 * called.
 */
FrSimIfaceTriv::FrSimIfaceTriv(const FrSimIfaceTriv& sift) {
  simDoneFlag = 0;
}

/**
 * Assignment
 * Assignment operator is private since this is a singleton class. It should
 * never be called.
 */
void FrSimIfaceTriv::operator=(const FrSimIfaceTriv& sift) {
  simDoneFlag = 0;
}

FrSimIfaceTriv::~FrSimIfaceTriv() {}

/**
 * Returns the single instance of this class. This call manages the singleton
 * aspect of this class. Since the constructor is private you must use this call
 * to get access to any instance of this class.
 */
FrSimIfaceTriv* FrSimIfaceTriv::getSimIface() {
  if(sif==0) {
    sif = new FrSimIfaceTriv();
  }
  return dynamic_cast<FrSimIfaceTriv*>(sif);
}

/******************************************************************************/

/**
 * This call initializes the simulation threading system and starts the first
 * thread running the main test program.
 * This function is ONLY called from the simulator thread.
 */
void FrSimIfaceTriv::startUserProgram(void*(*testMain)(void*)) {
  // This would mean that the FrSimTestMain header must be part of libfrsim!
  // FrSimTest would also have to be part of libfrsim! There's no way to extend
  // the main test base for each testbench to hold it's transactors.
  //foo = new FrSimTestMain();
  // FIXME: just let the user pass the main routine in to this call as a pointer
  // to a C function for now
  mainTestThread = new FrSimThread("Main Test", testMain, 0);
  mainTestThread->spawn();

  // Starting the first thread will also create the threadTracker. We should
  // keep a copy of that threadTracker because we use it to call 
  // waitAllThreadsBlocked() quite often.
  stt = FrSimThreadTracker::getThreadTracker();
  stt->waitAllThreadsBlocked();
}

/**
 * Actually start the simulator loop
 *
 * This simulator core loop needs to be updated. First, conds should be evaluated
 * and only those which had a signal from their sensitivity list just change
 * should be pulled out in a separate list. That list should be evaluated one at
 * a time to see if the condition test() call passes. Conds with a time sensitivity
 * to the current sim time should also be evaluated with test().
 */
void FrSimIfaceTriv::runSimulation() {
  // Loop forever to simulate
  for(;;) {
    // The simulation is over if there are no waitConds in the list or if the stop
    // flag is set. Return from this function in that case.
    if(conds.empty() || simDoneFlag==1) return;

    // loop through the simConds, evaluating each. Remove any that pass from the
    // list and unblock their thread, immediately waiting for all threads blocked
    // again.
    // FIXME: the eval list of conds can't be the same as the list that new conds are being created in!!!
    for(std::list<FrSimCondInfo>::iterator p(conds.begin()); p!=conds.end(); p++) { // loop through all entries in conds
      if(p->cond->test()) { // FIXME: conditions should only be evaluated in a cycle where one of the sensitive sigs change value!
	// This condition passes! remove it from the list and unblock it's thread!
	FrSimCondInfo cinfo = *p;
	conds.erase(p);
	cinfo.thr->trigger(); // FIXME: new conditions shouldn't be able to be met in this timestep, but this way they can be!
	stt->waitAllThreadsBlocked();
      }
    }

    // At the very end of the timestep, update all non-blocking assigns to FrSimCRegs
    for(std::map<std::string, FrTrivSimSig*>::iterator p(sigMap.begin()); p!=sigMap.end(); p++) {
      p->second->update();
      printf("Updating signal %s\n", p->first.c_str());
    }

    // Increase global sim time
    simTime++;

  }
}

/******************************************************************************/
/******************************************************************************/

/**
 * Set a flag which causes the simulator to stop executing at the end of this
 * timestep.
 */
void FrSimIfaceTriv::stop() {
  finish();
}

/**
 * Set a flag which causes the simulator to stop executing at the end of this
 * timestep.
 * This can be called from any thread at any time so it must be threadsafe.
 */
void FrSimIfaceTriv::finish() {
  simDoneFlag = 1;
}

/**
 * Probably does nothing for now.
 */
void FrSimIfaceTriv::restart() {
  finish();
}

/**
 * Returns the current simulation time. 
 * Should probably return a type of FrSimTime or something similar.
 * Could just return an int in the short term.
 */
void FrSimIfaceTriv::getTime(FrSimTime* t) {
  // FIXME: should set differently depending on the type field, but just return
  // sim time for now and ignore the scaled real value.
  t->high = 0;
  t->low = simTime;
}

int FrSimIfaceTriv::getTime() {return simTime;} // FIXME: return actual simulation time!

/******************************************************************************/
/******************************************************************************/

/**
 * Create and initialize the signal if it doesn't exist. Return the reference
 * for the signal whether it already existed or was just created.
 * Signals are initialized to X in every bit position.
 * If the width isn't already set in the simReg, use width of 1.
 * Different SimRegs can be connected if they are named the same thing. If named
 * the same (and have the same width) then only a single vecval array is created
 * and they all have a pointer to the same vecval.
 * Any chance that will cause problems with being threadsafe? Of course, if 
 * different processes are driving the same signal without a protocol then that
 * is inherently unsafe.
 *
 * The data structure here may need to be a little more complicated than I was
 * originally thinking. Each connected reg needs a refid to represent it's 
 * driver. Each connected reg also needs a pointer to a resolver struct for that
 * signal. If there is only one output, then it's a trivial resolver. If there
 * are multiple outputs, then things get more complicated.
 *
 * I want drives to act like non-blocking drives in verilog. That is, I want to 
 * schedule all the drives for a sim time and have them occur at the end of that
 * time when other changes can't take place because of them.
 */
void FrSimIfaceTriv::connect(FrSimCReg* r) {
  // check the reg to see if it is properly defined and set up (name, width, etc)
  int refId = sigList.size();
  r->setRefId(refId);
  printf("FrSimIfaceTriv: connecting signal %s, refId %d\n", r->getName(), r->getRefId());
  FrSimRegConn rcon;

  // check to see if the name already exists in the list, just connect if so
  // connected signals still have their own shadow reg since all drivers can 
  // create multiple events. I could have a resolver function in such cases.
  // match the name in sigMap
  std::map<std::string, FrTrivSimSig*>::iterator p = sigMap.find(r->getName());
  if(p == sigMap.end()) {
    // create new FrTrivSimSig and add to map
    rcon.sig = p->second; // set to pointer for existing FrTrivSimSig
    rcon.did = 0; // fixme: set to zero unless it drives the simulator; look at FrSimCReg direction to determine

  } else {
    // put the found pointer to this FrTrivSimSig in this FrSimRegConn
    rcon.sig = new FrTrivSimSig(r->getVecvalArraySize(), r->getName());
    rcon.did = 0; // fixme: set to zero unless it drives the simulator; look at FrSimCReg direction to determine
    
  }

  // every connections gets it's own struct FrSimRegConn in the sigList vector
  // How do I keep track of all the nxt values associate with one name???
  // Maybe only each unique name gets a FrSimRegCon
  sigList.push_back(rcon);

}

/**
 * Probably does nothing for this trivial simulator. If I kept a reference count
 * for connects then I could release the refid entry when the last disconnect
 * was made.
 * Disconnect is only called by the simRegs destructor, so I don't have to worry
 * about a connection continuing to be used after disconnect, I think.
 * 
 * Can't remove the entry from the sigList because that would mess up all refids
 */
void FrSimIfaceTriv::disconnect(FrSimCReg* r) {
  // nop is fine for the trivial simulator
}

/**
 * Update the FrSimReg with the current value in the simulator.
 * In the trivial simulator, this is a no-op because the current simulator value
 * is kept in the FrSimReg.
 *
 * Not exactly. What if two FrSimRegs are connected via the trivial simulator?
 * They would be connected by name. Initially, one signal could drive and 
 * multiple could sample. Thus the sample function has to work and has to use
 * the refId to get the current value and assign it to the given FrSimCReg.
 *
 * I suppose that ALL simRegs connected to a name could be constantly updated,
 * but that really isn't good. Some will only ever drive the simulator, and
 * others will only ever sample. Probably better to only resolve signals when
 * they are sampled rather than every timestep like I was originally thinking.
 */
void FrSimIfaceTriv::sample(const FrSimCReg* r) {
  printf("FrSimIfaceTriv: sampling signal %s, refId %d\n", r->getName(), r->getRefId());
  FrTrivSimSig* s = sigList[r->getRefId()].sig;
  FrSimRegVecVal* vv = s->sample();
  r->setVecvalArray(vv);
}

/**
 * Set the current value for a signal in the simulator.
 * Actually, the next value for the signal is set--this represents a non-blocking assign
 */
void FrSimIfaceTriv::drive(const FrSimCReg* r) {
  printf("FrSimIfaceTriv: driving signal %s, refId %d\n", r->getName(), r->getRefId());
  FrTrivSimSig* s = sigList[r->getRefId()].sig;
  FrSimRegVecVal* vv = r->getVecvalArray();
  s->drive(vv, 1);
}

/**
 * Push a simCondition onto the queue of conditions we're waiting for along with
 * a reference to the thread that called wait.
 */
void FrSimIfaceTriv::wait(FrSimCond c) {
  FrSimThread* me = FrSimThread::getSelf();
  FrSimCondInfo cinfo;
  cinfo.cond = &c;
  cinfo.thr = me;

  // add the cinfo structure onto the list
  conds.push_back(cinfo);
}



