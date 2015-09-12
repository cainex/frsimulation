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
#include "FrSimReg.h"

// This is a simple test program for the FrSimReg class. Maybe I should use a
// standard test generator harness, but this is easy for now and I can port to
// something standard in the future.

// Tests:
// create a reg using width and value
// create a reg using a string literal
// create a reg using another reg

// assign a reg to another reg of the same size
// assign a constant int to a reg
// assing a string literal to a reg
// assign a slice of a reg to another full reg
// assign a full reg to a slice of a reg
// assign a slice to a slice
// assing a constant to a slice
// assign a string literal to a slice
// assign slices such that lhs and rhs need to shift +/-1 to +/-100 bits

// assign and get x and z values from a reg

void testShift() {
  FrSimReg shiftreg(72,15);
  FrSimReg logicreg(72,15);

  printf("shiftreg: %s\n", shiftreg.binString().c_str());
  for(int i=0; i<68; i++) {
    shiftreg <<= 1;
    printf("shiftreg: %s\n", shiftreg.binString().c_str());
  }

  logicreg |= shiftreg;
  for(int i=0; i<68; i++) {
    shiftreg >>= 1;
    printf("shiftreg: %s\n", shiftreg.binString().c_str());
  }
  printf("logicreg: %s\n", logicreg.binString().c_str());
}

int main(int argc, char** argv) {
  printf("Hello World\n");
  FrSimReg foo(32,655349); // 32 bit reg with value 32'd655349
  FrSimReg bar("72'h123412deaddeadbeef");
  FrSimReg bas(foo);

  printf("Regs are %s, %s, %s\n", foo.hexString().c_str(), bar.hexString().c_str(), bas.hexString().c_str());
 
  bas = 7830934;
  foo = bas;
  //foo += 45676; // this wasn't doing the right thing before anyway... need to overload += for int on FrSimCReg
  bar = "72'hf0f0a5a50067903540";

  printf("Regs are %s, %s, %s\n", foo.hexString().c_str(), bar.hexString().c_str(), bas.hexString().c_str());

  // FIXME: I guess that with Dan's changes you can no longer assign to a slice of an FrSimReg...
  bar(63,32) = foo; // error: passing 'const FrSimReg' as 'this' argument of 'FrSimReg& FrSimReg::operator=(FrSimReg&)' discards qualifiers
  bas(15,0) = 0; // error: passing 'const FrSimReg' as 'this' argument of 'FrSimReg& FrSimReg::operator=(int)' discards qualifiers
  bar(31,16) = bas(15,0); // error: passing 'const FrSimReg' as 'this' argument of 'FrSimReg& FrSimReg::operator=(const FrSimReg&)' discards qualifiers
  foo = bar(31,0);

  printf("Regs are %s, %s, %s\n", foo.hexString().c_str(), bar.hexString().c_str(), bas.hexString().c_str());

  foo = "32'hzzzzzzzz";
  bar = (FrSimReg("40'habcdabcd11"), bas);

  printf("Regs are %s, %s, %s\n", foo.hexString().c_str(), bar.hexString().c_str(), bas.hexString().c_str());


  //testShift();

  return 0;
}

