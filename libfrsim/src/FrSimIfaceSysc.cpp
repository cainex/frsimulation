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

#include "FrSimIfaceSysc.h"

//*****************************************************************************
// FrSim Sysc Sysc signal wrapper base class
//*****************************************************************************
FrSimSyscSignalBase::FrSimSyscSignalBase(std::string name) :
  name(name)
{
}

FrSimSyscSignalBase::~FrSimSyscSignalBase()
{
}
//*****************************************************************************

//*****************************************************************************
// FrSim Sysc sc_signal_rv<> wrapper
//*****************************************************************************
template <int W>
FrSimSyscSignalRV<W>::FrSimSyscSignalRV(std::string name, sc_signal_rv<<W> *sig):
  FrSimSyscSignalBase(name),
  sig(sig)
{
}

template <int W>
FrSimSyscSignalRV<W>::~FrSimSyscSignalRV()
{
}

template <int W>
void FrSimSyscSignalRV<W>::read(FrSimRegVecVal *vv)
{
  sc_lv<w> val = sig->read();

  for (int i = 0; i < sig->length(); i++) {
    if ( val[i] == sc_dt::Log_0 ) {
      vv[i/31].aval &= (~(0x1 << i));
      vv[i/31].bval &= (~(0x1 << i));
    }
    else if ( val[i] == sc_dt::Log_1 ) {
      vv[i/31].aval |= ((0x1 << i));
      vv[i/31].bval &= (~(0x1 << i));
    }
    else if ( val[i] == sc_dt::Log_X ) {
      vv[i/31].aval &= (~(0x1 << i));
      vv[i/31].bval |= ((0x1 << i));
    }
    else if ( val[i] == sc_dt::Log_Z ) {
      vv[i/31].aval |= (~(0x1 << i));
      vv[i/31].bval |= ((0x1 << i));
    }

  }
}

template <int W>
void FrSimSyscSignalRV<W>::write(FrSimRegVecVal *vv)
{
  sc_lv<W> val;

  for (int i = 0; i < sig->length(); i++) {
    int aval = (vv[i/31].aval >> i) & 0x1;
    int bval = (vv[i/31].bval >> i) & 0x1;

    if (aval == 0 && bval == 0) {
      val[i] = sc_dt::Log_0;
    }
    else if (aval == 1 && bval == 0) {
      val[i] = sc_dt::Log_1;
    }
    else if (aval == 0 && bval == 1) {
      val[i] = sc_dt::Log_X;
    }
    else if (aval == 1 && bval == 1) {
      val[i] = sc_dt::Log_Z;
    }
  }

  sig->write(val);

}
//*****************************************************************************

//*****************************************************************************
// FrSim Simulator Iface for SystemC
//*****************************************************************************
FrSimIfaceSysc::FrSimIfaceSysc() :
  sc_module("FrSimModule")
{
}

FrSimIfaceSysc::FrSimIfaceSysc(const FrSimIfaceSysc &sift) {
}


void FrSimIfaceSysc::operator=(const FrSimIfaceSysc &sift) {
}

FrSimIfaceSysc *FrSimIfaceSysc::getSimIface() {
  if (sif == NULL) {
    sif = new FrSimIfaceSysc();
  }
  return dynamic_cast<FrSimIfaceSysc*>(sif);
}

void FrSimIfaceSysc::startUserProgram(void*(*testMain)(void *)) {
  stt = FrSimThreadTracker::getThreadTracker();
  mainTestThread = new FrSimThread("Main Test", testMain, 0);
  mainTestThread->spawn();
  stt->waitAllThreadsBlocked();
}

void FrSimIfaceSysc::stop() {
  sc_stop();
}

void FrSimIfaceSysc::finish() {
}

void FrSimIfaceSysc::getTime(FrSimTime *t) {
  t->type = 0;
  t->high = 0;
  t->low = 0;
  t->real = 0;
}

int FrSimIfaceSysc::getTime() {
  return 0;
}

void FrSimIfaceSysc::connect(FrSimCReg* r) {
  for (std::vector<FrSimSyscSignalBase*>::iterator i = sigList.begin();
       i != sigList.end();
       i++) {
    if (r->getName() == (*i)->name) {
      r->ref = i - sigList.begin();
      break;
    }
  }

  if (i == sigList.end()) {
    printf("Could not connect to signal name %s\n", r->getName());
    exit(1);
  }
}

void FrSimIfaceSysc::disconnect(FrSimCReg *r) {
}

void FrSimIfaceSysc::sample(const FrSimCReg *r) {
  r->setVecvalArray(sigList[r->reg]->read());
}

void FrSimIfaceSysc::drive(const FrSimCReg *r) {
  sigList[r->reg]->write(r->getVecvalArray());
}

void FrSimIfaceSysc::wait(FrSimCond c) {
  // Need to spawn an sysc dynamic thread
  // Sysc Thread does:
  //   1) wait for value_change_event of signal in c
  //   2) test condition, if not met return to 1)
  //   3) trigger FrSim thread passed in
  // do a wait on the FrSim thread

  FrSimThread *thr = FrSimThread::getSelf();
  sc_spawn(sc_bind(&FrSimIfaceSysc::syscSignalEvent, sc_reg(&c), sc_ref(thr)));
  thr->wait();
}

void FrSimIfaceSysc::syscSignalEvent(FrSimCond *c, FrSimThread *th)
{
  wait(sigList[c->sigList[0]->ref].get_
}
//*****************************************************************************

