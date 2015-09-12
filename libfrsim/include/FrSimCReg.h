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

#ifndef FRSIMCREG_H
#define FRSIMCREG_H

#include "FrSimReg.h"
class FrSimIface;

/**
 * Now called "Connected Sim Reg" so that normal regs can be sim regs
 * This simple class supports the connection between FrSimReg's and a FrSimIface.
 * It allows a FrReg (now called a FrSimReg) to be directly associated with a 
 * value in some sort of simulation so that wen the FrSimReg is assigned, the
 * value in the simulation is updated and when the FrSimReg is read, the value
 * is actually read from the simulation. The simulation can be any type that is
 * supported by a decendent of FrSimIface which could include verilog, VHDL, 
 * SystemC, custom C simulators, hybrids like Verilator, or almost any other 
 * two or four state digital logic simulator.
 */
class FrSimCReg : public FrSimReg {
 public:
  // Constructors
  // I think I'll need to support a few extra forms of the constructor, or at 
  // least several optional arguments.
  // Should be able to specify register size, direction, 2/4 state logic.
  FrSimCReg(const char* n, int size, int dir, int type);
  ~FrSimCReg();

  // Operators from the baseclass don't just automatically work!
  // Will I have this same problem with +,-,>,<,==,etc?
  void operator=(const FrSimReg& r);
  void operator=(const FrSimCReg& r);
  void operator=(const int v);
  void operator=(const char* v);
  //bool operator==(const int v);

  // There should probably be a method to set this directly from a vecval.
  // Need to take a closer look at sample and drive in the sif

  char* getName() const {return name;}
  int getRefId() const {return ref;}
  void setRefId(int refid) {ref=refid;}

  // Right now the sif assumes this is public, but I should provide accessor
  // functions instead of reaching in to set it.
  int ref;

 protected:
  // This class will also override the functions which update the reg's value.
  void readSync() const;
  void writeSync() const;

 private:
  FrSimIface* sif;
  char* name;
  int size; // really the same as widthbits in the base class
  int dir;
  int type; // 2 state/4 state logic, etc.
};

#endif

