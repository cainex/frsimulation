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
#include "FrSimThread.h"

// Instance several threads
// Each thread blocks it's self, main thread unblocks in some order
// Print out the thread list as the test progresses, check consistancy

// FIXME: this is really completely different since Dan changed things to pth
// FIXME: just rewrite this test from scratch

void* thr1(void*) {
  printf("Now starting thread1\n");
  //FrSimThread* me = FrSimThread::getSelf();
  //me->block();
  // do other stuff
  return 0;
}

void* thr2(void*) {
  printf("Now starting thread2\n");
  //FrSimThread* me = FrSimThread::getSelf();
  //me->block();
  // do other stuff
  return 0;
}

void* thr3(void*) {
  printf("Now starting thread3\n");
  //FrSimThread* me = FrSimThread::getSelf();
  //me->block();
  // do other stuff
  return 0;
}

void* thr4(void*) {
  printf("Now starting thread4\n");
  //FrSimThread* me = FrSimThread::getSelf();
  //me->block();
  // do other stuff
  return 0;
}

int main(int argc, char** argv) {
  //FrSimThread* t1 = new FrSimThread("thread1", thr1, 0);
  //t1->unblock();
  return 0;
}
