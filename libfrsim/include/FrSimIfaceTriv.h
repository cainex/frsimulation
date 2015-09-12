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

#ifndef FRSIMIFACETRIV_H
#define FRSIMIFACETRIV_H

#include <vector>
#include <list>
#include "FrSimIface.h"
#include "FrSimThread.h"

class FrTrivSimSig {
 public:
  FrTrivSimSig(int w, char* name); // width in words, name is just for debug
  int connect(); // returns driver id, only drivers need connect, fail if more than one connect for now
  void drive(FrSimRegVecVal* val, int id);
  FrSimRegVecVal* sample() {return curval;}
  void disconnect(int id) {} // only drivers need disconnect, not supported initially
  void update();
 private:
  // eventually need curval and nxtval for every driver and a resolver function,
  // but I'm limiting to one driver per sig for now so things are simpler
  FrSimRegVecVal* curval;
  FrSimRegVecVal* nxtval;
  bool connected;
  int width;
};

struct FrSimRegConn {
  FrTrivSimSig* sig;
  int did;
};
struct FrSimCondInfo {
  FrSimCond* cond;
  FrSimThread* thr;
};

/**
 * This class extends FrSimIface to implement a trivial simulator which only 
 * has a single toggling signal called clock. This is to allow debug of user
 * threads in a small, simple, and safe environment.
 *
 * The theory here is that you'll call FrSimIfaceTriv::getSimIface() once to set
 * up the singleton object and start the user thread and simulator loop. The 
 * user threads will only ever call FrSimIface::getSimIface(). Maybe it's not
 * worth the bother and I should just have a normal constructor for this class.
 * There would be a runtime casting problem if you ever tried to constuct 
 * another type of simIface after one had been set up. So there is a little 
 * extra error checking if all derived classes are singletons.
 *
 * I'm still not actually sure that what I'm proposing will compile, so I should
 * try to check it fairly quickly.
 */
class FrSimIfaceTriv : public FrSimIface {
 public:
  // This class is a singleton used to talk to the simulator from many different
  // threads. The get call returns the singular instance, creating it first if
  // necessary.
  static FrSimIfaceTriv* getSimIface();
  ~FrSimIfaceTriv();

  // Called only by startup code
  void startUserProgram(void*(*testMain)(void*));
  void runSimulation();

  // The next few calls support basic high-level interaction between the 
  // simulator and the user threads.
  void stop();
  void finish();
  void restart();
  void getTime(FrSimTime* t);
  int getTime();

  // The next few calls support simulator signal access by verif code.
  void connect(FrSimCReg* r);
  void disconnect(FrSimCReg* r);
  void sample(const FrSimCReg* r);
  void drive(const FrSimCReg* r);

  // This call enables synchronization between the verif code and simulator.
  void wait(FrSimCond c);

 protected:
  // These are just to get me thinking about how to construct the simulator
  void startNewTimestep();
  void findMatchingConds();
  void advanceDeltaTime();

 private:
  // Singleton class, so the constructor, copy constructor, and assignment
  // constructor are private. Maybe that doesn't matter since the derived class
  // will never be visible to user code. Maybe I should just use the normal 
  // constructor to build it but always get it as it's base class with the
  // getSimIface call in the base class.
  // Otherwise I have to have multiple pointers to the singleton object, or cast
  // it when it is returned because it's always stored as the base class.
  FrSimIfaceTriv();
  FrSimIfaceTriv(const FrSimIfaceTriv& sift);
  void operator=(const FrSimIfaceTriv& sift);

  // List of all signals connected to the simulator. Indexed by ref id.
  // contents are value for the trivial simulator. Assignments are always made
  // to the shadow copy, which is used to update the main list at the end of 
  // each timestep. Main list and shadow list have the same id's.
  // These lists are updated by connect() and disconnect()
  // Maybe make it a vector of struct simRegPair with elements current and next.
  std::vector<FrSimRegConn> sigList;
  std::map<std::string, FrTrivSimSig*> sigMap;

  // List of sim conditions that anybody is waiting on, and the thread that
  // called for the wait. Corresponding id's are the matching condition and 
  // thread from the same wait() call.
  // These lists are updated by wait()
  // This would probably be much better as a single list of a new struct which
  // holds both the condition and the thread. A list because I must randomly
  // remove any elements when the conditions pass. New conditions can be added
  // back in anywhere.
  std::list<FrSimCondInfo> conds;
  //std::vector<FrSimThread> condThreads;
  //pthread_mutex_t simCondsMutex;

  // Current simulation time; units are arbitrary; may need delta time later
  int simTime;
  int simDoneFlag;
  // A pointer to the threadTracker, just because we use it to call
  // waitAllThreadsBlocked() quite often
  FrSimThreadTracker* stt;
  FrSimThread* mainTestThread;
};
#endif



