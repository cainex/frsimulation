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

#include <stdio.h>
#include "FrSimIfaceTriv.h"

void* test(void*) {
  printf("Hello world from main test thread\n");
  //FrSimCReg* r1 = new FrSimCReg("test1", 1, 0, 0);
  
  FrSimIface *sif = FrSimIface::getSimIface();
  FrSimCReg *clk = new FrSimCReg("ivltest.clk", 1, 0, 0);
  FrSimCReg *val = new FrSimCReg("ivltest.val", 1, 1, 0);
  //FrSimThread *me = FrSimThread::getSelf();
  for(int i=0;i<10;i++) {
    //change c = change(clk);
    sif->wait(posedge(clk)); // FIXME: should be reference, not object! The condition won't live beyond the wait call, right?
    //FrSimTime now;
    //sif->getTime(&now);
    FrSimReg clkreg = *clk;
    printf("Current Time is %d, clk value is %d\n", sif->getTime(), clk->to_int());
    *val = *clk; // FIXME: causes a "FrSimThreadTracker thread spin detector!"
  }
  //*val = FrSimReg(1,1);
  return NULL;
  
  return 0;
}

int main(int argc, char** argv) {
  if (!pth_init()) {
    printf("FrSimIface: can't init pth library in frsim_main_wrapper()\n");
    exit(1);
  }

  FrSimIfaceTriv* sif = FrSimIfaceTriv::getSimIface();
  // These other calls really could just be called by the constructor
  sif->startUserProgram(test);
  sif->runSimulation();
  printf("Simulation done, returning...\n");
  return 0;
}


