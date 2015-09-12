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

#ifndef FRSIMIFACE_H
#define FRSIMIFACE_H
#include "FrSimCond.h"
#include "FrSimCReg.h"
#include "FrSimTime.h"

/**
 * This class provides a generic interface to a simulator. The simulator might
 * be verilog and use the verilog PLI to connect, or it might be a C simulation
 * or it might be VHDL or matlab or any number of other platforms. The 
 * assumptions made here is that there are signals which may be sampled or
 * driven in the simulation, and that there is a time measurement, and that
 * there are conditions on those signals that may be waited for to cause time to
 * advance until those conditions are met.
 *
 * Additionally, this class assumes a multi-threaded environment that is managed
 * by an FrSimThreadTracker. It assumes that there will be one or more user test
 * programs each running in their own thread or in multiple threads that will
 * each drive, sample, and synchronize with signals in the simulator.
 */
class FrSimIface {
 public:
  // This class is a singleton used to talk to the simulator from many different
  // threads. The get call returns the singular instance, creating it first if
  // necessary.
  static FrSimIface* getSimIface();
  virtual ~FrSimIface() {}

  // The next few calls support basic high-level interaction between the 
  // simulator and the user threads. These might be called by user code, but
  // only top level control code. Normal routines wouldn't have to control
  // simulator function or track what's going on in time.
  // Does anything need to be done to get the simulator into a stable state
  // before calling any of these?
  virtual void stop() = 0;
  virtual void finish() = 0;
  virtual void restart() = 0;
  virtual void getTime(FrSimTime* t) = 0;
  virtual int getTime() = 0;

  // These four methods are generally only called from the SimReg class. They
  // allow direct access to signals in the simulation. I have to determine how
  // to pass signal values. Probably as another helper class: FrVecVal
  // The thing that's missing here is the idea of a synchronous clocked 
  // interface which could likely be used to improve performance.
  virtual void connect(FrSimCReg* r) = 0;
  virtual void disconnect(FrSimCReg* r) = 0;
  // These two might want to return int for an error code.
  // There might be a few versions of sample and drive to support 2 state and
  // 4 state and other types of logic. What about vectors larger than 32 bits?
  // I think there needs to be an index here as well.
  // What about updating attributes like change for when a reg gets an event
  // that it is sensitive to??
  virtual void sample(const FrSimCReg* r) = 0;
  virtual void drive(const FrSimCReg* r) = 0;

  // This is half of the sync procedure. The user thread calls wait in order to
  // pause it's self until the given expression is true. 
  // Threads must be cancelable when they are waiting, but that may be the only
  // time they have to be cancelable.
  // Wait should register with the simulator for when to test if it's done, and
  // then block it's thread so that the sim can unblock it when it is called by
  // the simulator and the expression passes.
  // This call is always called from a user thread. This is the same as teal at.
  // I almost want to make it a global function just to avoid the user having to
  // get access to the simIface just to call it.
  // This call must be thread safe because every test thread will be sitting in
  // this call and waiting on a mutex or condition whenever the simulator thread
  // is running.
  virtual void wait(FrSimCond c) = 0;
  //void wait(change c) {FrSimCond p = c; wait(p);}
 protected:
  // Reference to the single SimIface; must be initialized to zero in imp code
  static FrSimIface *sif;

  //private:
  // Singleton class, so the constructor, copy constructor, and assignment
  // constructor are protected, but do nothing.
  FrSimIface();
  FrSimIface(const FrSimIface& si);
  FrSimIface& operator=(const FrSimIface& si);

};

#endif


