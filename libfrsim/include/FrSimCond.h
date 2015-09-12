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

#ifndef FRSIMCOND_H
#define FRSIMCOND_H
#include <vector>
#include <iostream>
#include "FrSimCReg.h"
#include "FrSimTime.h"

/**
 * FrSimCond -- Base class for conditions. 
 * This is the base class for all the things a wait expression can check for.
 * Basically, wait() can wait for a condition to occur in a simulation. An
 * FrSimCond is a flexible way to specify a simulation condition and test if it
 * occurs. It also supports the operation by supplying information about when
 * you need to (re)test to see if the condition is now true.
 * It is basically a test to see if an arbitrary and/or combination of events
 * and state in the simulator is true.
 *
 * This is kinda a hack for now. The FrSimCond its self holds everything needed 
 * to evaluate the condition, but the derived classes will just be used for their
 * constructors to cleanly construct the appropriate FrSimCond. The reason I'm 
 * doing this is that I can't seem to construct a subclass in a wait statement
 * right now and have it properly call the proper method in the subclass--it 
 * always calls the method in the base class for some reason. It will work right
 * if I pass in pointers, but that has it's own problems because you have to call
 * new inside the wait statement expression. 
 */

class FrSimCond {
 public:
  enum condType{Change, Posedge, Negedge};
  /**
   * Constructor
   */
  FrSimCond();
  
  /**
   * Destructor
   */
  virtual ~FrSimCond();
  	
  /**
   * test
   * Pure virtual method which is called to determine if the condition is 
   * satisfied.  This method is overloaded by its children to test for 
   * specific conditions
   * @return True/False
   */
  virtual bool test(); // return true if the condition is met. Could also be called check() for check condition
  	
  /**
   * sigs
   * Gets a vector<> of signals which need to be monitored for this condition.
   * Purely virtual, must be overloaded by specific condtions.
   * @param sigList reference to vector<> to return list to.
   */
  //virtual void sigs(std::vector<FrSimReg *> &sigList) {}
  	
  /**
   * times
   * Gets a vector<> of times which need to be monitored for this condition
   * Purely virtual, must be overloaded by specific condtions.
   * @return a vector with pointers to all relevant FrSimTime objects
   */
  virtual void times() {} // return a list of times to run test() when they are reached. Not sure of the format.

  /**
   * List of signal pointers which are relevant for this condition
   * This is left public for now so that the list can be easily retrieved
   * when needed by the wait function in the sim interface. I originally 
   * intended to use the sigs() method above to return the list, but that
   * seemed like it might have a lot of overhead, returning a struct.
   */
  std::vector<FrSimCReg *> sigList;
  FrSimCReg *sensReg;
  FrSimReg lastVal;
  condType op;
 private:
  /**
   * Running ID generator, generates a new ID for next callback;
   * I don't think this is currently used--what is the intent?
   */
  //int nextCallbackID;
};

/**
 * change condition
 * Checks for any value change on the specified signal
 */
class change : public FrSimCond {
 public:
  /**
   * Constructor
   * @param r pointer reference to the signal be watched for change
   */
  change(FrSimCReg *r);
  	
  //bool test(); // simIface must be able to report a change attribute on a SimReg
  //void sigs(std::vector<FrSimReg *> &sigList); // really should return a list of int (ref id). Returns a list of sigs to run test() when they change
  //void times();
 private:
  //FrSimReg *reg;
  //bool isSynced;
};

/**
 * posedge condition
 * Checks for a positive edge transition on the specified signal
 * The signal being monitor must be of a single bit width.
 * @see FrSimCond
 */
class posedge : public FrSimCond {
 public:
  /**
   * Constructor
   * @param r pointer reference to signal being watch -- must be single bit width.
   */
  posedge(FrSimCReg *r);
  	
  //bool test(); // this is really an expression, but just hard coded
  //void sigs(std::vector<FrSimReg *> &sigList); // really should return a list of int (ref id). Returns a list of sigs to run test() when they change
  //void times();
 private:
  //FrSimReg *reg;
  //FrReg currentValue;
};

/**
 * negedge condition
 * Checks for a negative edge transition on the specified signal
 * The signal being monitor must be of a single bit width.
 * @see FrSimCond
 */
class negedge : public FrSimCond {
  /**
   * Constructor
   * @param r pointer reference to signal being watch -- must be single bit width.
   */
 public:
  negedge(FrSimCReg *r);

  //bool test();
  //void sigs(std::vector<FrSimReg *> &sigList); // really should return a list of int (ref id). Returns a list of sigs to run test() when they change
  //void times();
 private:
  //FrSimReg *reg;
  //FrReg currentValue;
};

/**
 * equal condition
 * Waits the the provided signal is equal to the provided value.
 * @see FrSimCond
 */
class equal : public FrSimCond {
 public:
  /**
   * Constructor
   * @param r pointer reference to signal being watched
   * @param c reference to a constant FrSimReg value to be compared to
   */
  equal(FrSimCReg *r, FrSimReg &c);
  	
  //bool test();
  //void sigs(std::vector<FrSimReg *> &sigList); // really should return a list of int (ref id). Returns a list of sigs to run test() when they change
  //void times();
 private:
  //FrSimReg *reg;
  //FrReg &val;
};

class notequal : public FrSimCond {
};

class after : public FrSimCond {
};

class before : public FrSimCond {
};

class during : public FrSimCond {
};


#endif


