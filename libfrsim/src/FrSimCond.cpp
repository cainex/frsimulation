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

#include "FrSimCond.h"
#include <iostream>
#include <cstdlib>

/******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// FrSimCond base class
////////////////////////////////////////////////////////////////////////////////

/**
 * FrSimCond constructor
 */
FrSimCond::FrSimCond() : lastVal(1,0) {
}

/**
 * FrSimCond destructor
 */
FrSimCond::~FrSimCond() {
}

bool FrSimCond::test() {
  FrSimReg tmpData(1,0); 
  tmpData = *sensReg;
  //FrSimRegVecVal *srvv = sensReg->getVecvalArray();
  //printf("Calling test for signal %s with value %d (%d/%d) lastVal is %d\n", sensReg->getName(), tmpData.to_int(),
  // srvv[0].aval, srvv[0].bval, lastVal.to_int());
  switch(op) {
  case Change:
    return true;
  case Posedge:
    if((lastVal) == FrSimReg(1, 0) && (tmpData) == FrSimReg(1, 1) ) {
      return true;
    } else {
      lastVal = tmpData;
      return false;
    }
  case Negedge:
    if((lastVal) == FrSimReg(1, 1) && (tmpData) == FrSimReg(1, 0) ) {
      return true;
    } else {
      lastVal = tmpData;
      return false;
    }
  };
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// FrSimCond change class
////////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 * Simple condition takes a single signal
 */
change::change(FrSimCReg *r) {
  op = Change;
  sensReg = r;
  sigList.push_back(sensReg);
}

/**
 * Test
 * Always returns true, since any change will satify the condition
 *
bool change::test() {
  return true;
}
*/
/**
 * sigs()
 * return a vector of signals
 * FIXME: no, No, NO! not what was intended
 * Currently, I don't use this method at all and just access sigList directly
 * when I need the signal list in simIface.wait()
 *
void change::sigs(std::vector<FrSimReg *> &sigList) {
  sigList.push_back(reg);
}
*/
/**
 * times()
 * return a vector of signals
 * FIXME: I currently don't support waiting for a fixed time, but will want to
 * add that.
 *
void change::times() {
}
*/
////////////////////////////////////////////////////////////////////////////////
// FrSimCond posedge class
////////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 * Posedge only supports signals of width 1
 *
posedge::posedge(FrSimReg *r) : reg(r), currentValue(r->getData()) {
  currentValue = r->getData();
  // only support posedge for single bit vectors
  if (currentValue.getWidthBits() != 1) {
    // FIXME: add proper error handling here
    exit(1);
  }
  sigList.push_back(reg);
}
*/
posedge::posedge(FrSimCReg *r) {
  op = Posedge;
  sensReg = r;
  lastVal = *r;
  // only support posedge for single bit vectors
  // FIXME: this isn't really checking; can't fail!
  if (lastVal.getWidthBits() != 1) {
    // FIXME: add proper error handling here
    exit(1);
  }
  sigList.push_back(sensReg);

}
/**
 * Test
 * Checks that previous value = 0 and current value = 1
 *
bool posedge::test() { 
  //std::cout << "posedge test received(" << reg->getData() << ")" << std::endl;
  FrSimReg tmpData = reg->getData();
  //std::cout << "receieved(" << tmpData << ") previous(" << currentValue << ")" << std::endl;
  if ((currentValue) == FrSimReg(1, 0) && (tmpData) == FrSimReg(1, 1) ) {
    //std::cout << "posedge true" << std::endl;
    return(true); 
  } else {
    //std::cout << "posedge false" << std::endl;
    (currentValue) = (reg->getData());
    return(false);
  }
}
*/
/**
 * sigs()
 * return a vector of signals
 *
void posedge::sigs(std::vector<FrSimReg *> &sigList) {
  sigList.push_back(reg);
}
*/
/**
 * times()
 * return a vector of signals
 *
void posedge::times() {
}
*/


////////////////////////////////////////////////////////////////////////////////
// FrSimCond negedge class
////////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 * negedge only supports signals of width 1 
negedge::negedge(FrSimReg *r) : reg(r), currentValue(r->getData()) {
  // only support negedge for single bit vectors
  if (currentValue.getWidthBits() != 1) {
    // FIXME: add proper error handling here
    exit(1);
  }
}
*/
negedge::negedge(FrSimCReg *r) {
  op = Negedge;
  sensReg = r;
  lastVal = *r;
  // only support posedge for single bit vectors
  if (lastVal.getWidthBits() != 1) {
    // FIXME: add proper error handling here
    exit(1);
  }
  sigList.push_back(sensReg);
}

/**
 * Test
 * Checks that previous value = 0 and current value = 1
 *
bool negedge::test() { 
  FrSimReg tmpData = reg->getData();
  if (currentValue == FrSimReg(1, 1) && (tmpData) == FrSimReg(1, 0) ) {
    return(true); 
  } else {
    currentValue = reg->getData();
    return(false);
  }
}
*/
/**
 * sigs()
 * return a vector of signals
 *
void negedge::sigs(std::vector<FrSimReg *> &sigList) {
  sigList.push_back(reg);
}
*/
/**
 * times()
 * return a vector of signals
 *
void negedge::times() {
}
*/

////////////////////////////////////////////////////////////////////////////////
// FrSimCond equal class
////////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 * equal only supports signals of width 1 */
equal::equal(FrSimCReg *r, FrSimReg &c) {
}

/**
 * Test
 * Checks that previous value = 0 and current value = 1
 *
bool equal::test() { 
  FrSimReg tmpData = reg->getData();
  if (tmpData == val) {
    return(true); 
  } else {
    return(false);
  }
}
*/
/**
 * sigs()
 * return a vector of signals
 *
void equal::sigs(std::vector<FrSimReg *> &sigList) {
  sigList.push_back(reg);
}
*/
/**
 * times()
 * return a vector of signals
 *
void equal::times() {
}
*/




