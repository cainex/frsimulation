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

#include <string.h>
#include "FrSimIface.h"
#include "FrSimCReg.h"

/**
 *
 */
FrSimCReg::FrSimCReg(const char* n, int size, int dir, int type):FrSimReg(size,0) {
  // FIXME: currently creating the base class as 0 bits wide, but that's broken
  // I think FrReg has to have a way to change it's width, or to not have a
  // width defined for some cases. Maybe a width of 0 if it's not yet specified?
  name = (char*)malloc(strlen(n)+2);
  strcpy(name, n);
  // get the FrSimIface instance
  sif = FrSimIface::getSimIface();

  // call connect from the FrSimIface; this will properly set the width
  // Except there's no public method to set the width after construction! Making
  // the classes friends seems excessive. Maybe I should just add that setWidth
  // method, which fails if width isn't set to zero or isn't called with the val
  // that width is already set to.
  sif->connect(this);
}

/**
 *
 */
FrSimCReg::~FrSimCReg() {
  // FIXME: need to deallocate vecval array; can I call the parents destructor?
  sif->disconnect(this);
}

void FrSimCReg::operator=(const FrSimReg& r) {
  FrSimReg::operator=(r);
}

void FrSimCReg::operator=(const FrSimCReg& r) {
  FrSimReg::operator=(r);
}

void FrSimCReg::operator=(const int v) {
  FrSimReg::operator=(v);
}

void FrSimCReg::operator=(const char* v) {
  FrSimReg::operator=(v);
}

//bool FrSimCReg::operator==(const int v) {
//  FrSimReg tmp(32,v);
//  return FrSimReg::operator==(tmp);
//}

/**
 *
 */
void FrSimCReg::readSync() const {
  sif->sample(this);
}

/**
 *
 */
void FrSimCReg::writeSync() const {
  sif->drive(this);
}





