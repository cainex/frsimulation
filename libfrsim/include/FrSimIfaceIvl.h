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

#ifndef FRSIMIFAVEIV_H
#define FRSIMIFAVEIV_H

extern "C" {
#include "pth.h"
#include "vpi_user.h"
#include "veriuser.h"
}
#include "FrSimIface.h"
#include "FrSimThread.h"
#include <iostream>
#include <vector>
#include <list>

// Helper struct for the SimIface
struct FrSimWaitInfo {
  FrSimCond *c; // so we can call test from the callback
  FrSimThread *t; // so we can restart the waiting thread from the callback
  std::vector<s_cb_data*> cbData; // kept so we can free the storage later
  std::vector<vpiHandle> cbHndls; // kept so we can unregister the callbacks
};

class FrSimIfaceIvl : public FrSimIface {
 public:
  static FrSimIfaceIvl* getSimIface();
  ~FrSimIfaceIvl();

  void startUserProgram(void*(*testMain)(void*));

  void stop();
  void finish();
  void restart();
  void getTime(FrSimTime *t);
  int getTime();

  // The next few calls support simulator signal access by verif code.
  void connect(FrSimCReg* r);
  void disconnect(FrSimCReg* r);
  void sample(const FrSimCReg* r);
  void drive(const FrSimCReg* r);

  // This call enables synchronization between the verif code and simulator.
  void wait(FrSimCond c);

 private:
  void registerSimCallbacks(FrSimCond *c, FrSimThread *t);
  void registerVpiCallback(vpiHandle h, FrSimWaitInfo *wi);

  FrSimIfaceIvl();
  FrSimIfaceIvl(const FrSimIfaceIvl& sift);
  void operator=(const FrSimIfaceIvl& sift);

  // A pointer to the threadTracker, just because we use it to call
  // waitAllThreadsBlocked() quite often
  FrSimThreadTracker* stt;
  FrSimThread* mainTestThread;
  std::vector<vpiHandle> sigList;
};

void removeSimCallbacks(FrSimWaitInfo *wi);
int vpi_condition_check_callback(s_cb_data *cb_data);
void frsim_main_wrapper();

extern void *frsim_main(void *);

#endif
