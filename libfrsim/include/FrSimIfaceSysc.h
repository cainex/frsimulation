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

#ifndef FRSIMIFACESYSC_H
#define FRSIMIFACESYSC_H

extern "C" {
#include "pth.h"
}
#include "FrSimIface.h"
#include "FrSimThread.h"


class FrSimSyscSignalBase {
 public:
  std::string name;

  FrSimSyscSignalBase(std::string name);
  ~FrSimSyscSignalBase();

  virtual FrSimRegVecVal *read() = 0;
  virtual void write(FrSimRegVecVal *vv) = 0;

};

template <int W>
class FrSimSyscSignalRV : public FrSimSyscSignalBase {
 public:
  sc_signal_rv<W> *sig;

  FrSimSyscSignalRV(std::string name, sc_signal_rv<W> *sig);
  ~FrSimSyscSignalRV();

  virtual void read(FrSimRegVecVal *vv);
  virtual void write(FrSimRegVecVal *vv);
    
};

class FrSimIfaceSysc : public sc_module, public FrSimIface {
 public:
  // This class is a singleton used to talk to the simulator from many different
  // threads. The get call returns the singular instance, creating it first if
  // necessary.
  static FrSimIfaceSysc* getSimIface();
  virtual ~FrSimIfaceSysc() {}

  virtual void stop();
  virtual void finish();
  virtual void restart();
  virtual void getTime(FrSimTime* t);
  virtual int getTime();

  virtual void connect(FrSimCReg* r);
  virtual void disconnect(FrSimCReg* r);
  virtual void sample(const FrSimCReg* r);
  virtual void drive(const FrSimCReg* r);

  virtual void wait(FrSimCond c);

  template <int W>
    void registerSyscSignal(sc_signal_rv<W> *newSig);

  void syscSignalEvent(FrSimCond *c, FrSimThread *th);

 protected:
  // Reference to the single SimIface; must be initialized to zero in imp code
  static FrSimIface *sif;

  //private:
  // Singleton class, so the constructor, copy constructor, and assignment
  // constructor are protected, but do nothing.
  FrSimIfaceSysc();
  FrSimIfaceSysc(const FrSimIfaceSysc& si);
  void operator=(const FrSimIfaceSysc& si);

  std::vector<FrSimSyscSignalBase*> sigList;
};

void frsim_main_wrapper();

extern void *frsim_main(void *);

#endif


