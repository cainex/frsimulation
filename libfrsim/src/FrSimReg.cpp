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
#include <sstream>
#include "FrSimReg.h"
#include <iostream>

/******************************************************************************/
// Construction/Destruction

/**
 * Construct a reg with width w and initial value v for the low 32 bits.
 * A reg with width w is actually defined as (w-1) downto 0. It isn't possible
 * to define a reg as other then (w-1) downto 0. Desired shifts, etc., must be
 * compensated for manually or in the simulator.
 * If the value was the first arg and the second was a default with of 32 or 64
 * then the compiler could automagically convert constants into FrSimRegs for use
 * in any function that uses an FrSimReg, like +/-/>/</&/| etc.
 */
FrSimReg::FrSimReg(int w, int v) :
	stringHeaderFormat(true),
	stringFormat(Hex)
{
  widthBits = w;
  widthWords = getWidthWordsFromBits(widthBits);
  allocVecValArray(widthWords);
  //operator=(v); // ASW temporarily replaced this line with code below for debug
  vecval[0].aval = v;
  vecval[0].bval = 0;
  for(int i=1; i<widthWords; i++) {
    vecval[i].aval = 0;
    vecval[i].bval = 0;
  }

}

/**
 * Construct a reg from a vecval array
 */
FrSimReg::FrSimReg(int vecSize, FrSimRegVecVal *vec) :
	stringHeaderFormat(true),
	stringFormat(Hex)
{
  widthBits = vecSize;
  widthWords = getWidthWordsFromBits(widthBits);
  allocVecValArray(widthWords);
  for (int i = 0; i < widthWords; i++) {
    vecval[i].aval = vec[i].aval;
    vecval[i].bval = vec[i].bval;
  }
  delete vec; // FIXME: ??? why would you delete a pointer passed in?
}

/**
 * Construct a reg from a string literal
 */
FrSimReg::FrSimReg(std::string val)  :
	stringHeaderFormat(true),
	stringFormat(Hex)
{
  // parse the string once to get the width and alloc the vecval array
  // then just call operator= which will parse the string again and assign it
  // For now:
  widthBits = evalStringLiteral(val);
  widthWords = getWidthWordsFromBits(widthBits);
  allocVecValArray(widthWords);
  operator=(val);
}

FrSimReg::FrSimReg(int w, std::string val)  :
	stringHeaderFormat(true),
	stringFormat(Hex)
{
  // parse the string once to get the width and alloc the vecval array
  // then just call operator= which will parse the string again and assign it
  // For now:
  widthBits = w;
  widthWords = getWidthWordsFromBits(widthBits);
  allocVecValArray(widthWords);
  operator=(val);
}
/**
 * Construct a reg which is identical to the existing reg r.
 */
FrSimReg::FrSimReg(const FrSimReg& r)  :
	stringHeaderFormat(true),
	stringFormat(Hex)
{
  widthBits = r.widthBits;
  widthWords = getWidthWordsFromBits(widthBits);
  allocVecValArray(widthWords);
  operator=(r);
}

FrSimReg::FrSimReg(int w, const FrSimReg& r)  :
	stringHeaderFormat(true),
	stringFormat(Hex)
{
  widthBits = w;
  widthWords = getWidthWordsFromBits(widthBits);
  allocVecValArray(widthWords);
  operator=(r);
}

/**
 * Construct a reg which is identical to the existing regSlice r.
 */
FrSimReg::FrSimReg(const FrSimRegSlice& r)  :
	stringHeaderFormat(true),
	stringFormat(Hex)
{
  widthBits = r.upperBitPos - r.lowerBitPos + 1;
  widthWords = getWidthWordsFromBits(widthBits);
  allocVecValArray(widthWords);
  operator=(r);
}

/**
 * Destructor
 * 
 */
FrSimReg::~FrSimReg() {
  delete[] vecval;
}

/******************************************************************************/
// Assignments

/**
 * Assign the value of a FrSimReg based on the value of another FrSimReg
 * Truncate r if it's bigger and zero-pad if r is smaller then the current reg.
 * Maybe allow assignment to wider or narrower regs, but have a state bit that
 * prints a warning when set, to aid in debug. The problem with that is that I
 * am now thinking about creating several class methods that will purposely 
 * assign regs of mismatched size, and I don't want to be warned about that
 * every time. I don't think I can make operator= take another argument,
 * though, which would specify an expected width mismatch.
 *
 * FIXME: I really need to be able to call readSync() when I construct or init
 * from another FrSimReg. Unfortunately, a const simReg doesn't like having
 * readSync() called on it. I either need to somehow call readSync on a const
 * FrSimReg (FrSimCReg) or create a complete copy in those cases and call
 * readSync() on that copy!
 */
FrSimReg& FrSimReg::operator=(const FrSimReg& val) {
  val.readSync();
  //if(widthBits != val.widthBits) { }
  //std::cout << "Setting const FrSimReg" << std::endl;
  int copySize = (widthWords<val.widthWords) ? widthWords : val.widthWords;
  for(int i=0; i<copySize; i++) {
    vecval[i].aval = val.vecval[i].aval;
    vecval[i].bval = val.vecval[i].bval;
  }
  for(int i=copySize; i<widthWords; i++) { // set extra words to zero, if any
    vecval[i].aval = 0;
    vecval[i].bval = 0;
  }
  clearUnusedBits(vecval, widthWords, widthBits);
  writeSync();
  return *this;
}

/**
 *
 */
FrSimReg& FrSimReg::operator=(FrSimReg& val) {
  //printf("Calling operator=(FrSimReg) with val=%d/%d\n", val.vecval[0].aval, val.vecval[0].bval);
  //std::cout << "Setting FrSimReg" << std::endl;
  //setValue(val); // calls readSync() and writeSync()
  val.readSync();
  //if(widthBits != val.widthBits) { }
  int copySize = (widthWords<val.widthWords) ? widthWords : val.widthWords;
  for(int i=0; i<copySize; i++) {
//    std::cout << " aval:" << val.vecval[i].aval << "  bval:" << val.vecval[i].bval << std::endl;
    vecval[i].aval = val.vecval[i].aval;
    vecval[i].bval = val.vecval[i].bval;
  }
  for(int i=copySize; i<widthWords; i++) { // set extra words to zero, if any
    vecval[i].aval = 0;
    vecval[i].bval = 0;
  }
  clearUnusedBits(vecval, widthWords, widthBits);
  writeSync();

  return *this;
}

/**
 * Assign the value of a FrSimReg based on an int or constant value
 * Since the constant can't be bigger then 32 bits, just assign the first vecval
 * word from the input value. Upper words (if any) get set to zero.
 */
FrSimReg& FrSimReg::operator=(const int val) {
  //printf("Calling operator=(int) with val=%d\n", val);
  vecval[0].aval = val;
  vecval[0].bval = 0;
  for(int i=1; i<widthWords; i++) {
    vecval[i].aval = 0;
    vecval[i].bval = 0;
  }
  writeSync();
  return *this;
}

/**
 * Assign the value of a FrSimReg based on a verilog style string literal
 * The width of the literal will not change the width of this FrSimReg, but if the
 * string value is too large to be stored in this reg then an exception will be
 * thrown: probably invalid arg or out of range or something similar.
 */
FrSimReg& FrSimReg::operator=(std::string val) {
  setToString(val);
  return *this;
}

/**
 * Assign the value of a FrSimReg based on a FrSimRegSlice
 */
FrSimReg& FrSimReg::operator=(const FrSimRegSlice& val) {
  // create a new reg which is a copy of val.reg and shift it right by lowerBitPos
  FrSimReg r(val.reg);
  r>>=val.lowerBitPos;
  operator=(r);
  return *this;
}

void FrSimReg::setValue(FrSimReg &val) {
  val.readSync();
  //if(widthBits != val.widthBits) { }
  int copySize = (widthWords<val.widthWords) ? widthWords : val.widthWords;
  for(int i=0; i<copySize; i++) {
//    std::cout << " aval:" << val.vecval[i].aval << "  bval:" << val.vecval[i].bval << std::endl;
    vecval[i].aval = val.vecval[i].aval;
    vecval[i].bval = val.vecval[i].bval;
  }
  for(int i=copySize; i<widthWords; i++) { // set extra words to zero, if any
    vecval[i].aval = 0;
    vecval[i].bval = 0;
  }
  clearUnusedBits(vecval, widthWords, widthBits);
  writeSync();
}

void FrSimReg::setValue(const FrSimReg &val) {
  val.readSync();
  //if(widthBits != val.widthBits) { }
  int copySize = (widthWords<val.widthWords) ? widthWords : val.widthWords;
  for(int i=0; i<copySize; i++) {
    vecval[i].aval = val.vecval[i].aval;
    vecval[i].bval = val.vecval[i].bval;
  }
  for(int i=copySize; i<widthWords; i++) { // set extra words to zero, if any
    vecval[i].aval = 0;
    vecval[i].bval = 0;
  }
  clearUnusedBits(vecval, widthWords, widthBits);
  writeSync();
}


/******************************************************************************/
// Slices
FrSimRegSlice FrSimReg::operator[](int b) {
  readSync();
  return FrSimRegSlice(*this, b, b);
}
 /**
  * Create a tmp class which just represents a slice of an existing reg to make 
  * assignments to slices much easier.
  */
FrSimRegSlice FrSimReg::operator()(int u, int l) { 
   readSync();
   return FrSimRegSlice(*this, u, l); 
 }

/**
 * Create a copy of a slice taken from a const reg
 * Algorithm to copy a slice of *this into new reg r:
 * - lsw of r is word l/32 right shifted by l%32 of *this
 * - iterate through words, but the msw of r is tricky because extra bits above
 *   u in this must be zeroed out.
 *
FrSimReg FrSimReg::operator()(int u, int l) const {
  readSync();
  int wb = u-l+1;
  FrSimReg r(wb, 0);
  FrSimReg c(*this);
  r =  (c >>= l);
  clearUnusedBits(r.vecval, r.widthWords, r.widthBits);

  return r;
}
*/
/******************************************************************************/
// Accessors

/**
 * This converts the reg to a 32 bit int, as near as it can. If there are any
 * x's or z's then it warn's and returns zero. If the reg is larger than can be
 * expressed in 32 bits, then it warns and returns the value of the low 32 bits.
 */
int FrSimReg::to_int() {
  readSync();
  if(vecval[0].bval!=0) {
    // FIXME: print warning; also warn if widthWords>1 and any upper bval != 0
    return 0;
  } else {
    // FIXME: print warning if widthWords>1 and aval or bval of any upper words
    // isn't equal to zero.
    clearUnusedBits(vecval, widthWords, widthBits); // was getting garbage in upper bits for some reason!
    return vecval[0].aval;
  }
}

/**
 * Return a string that represents the value of the reg in hex format. Any
 * nibble that contains a x or z will display a value of x. If all four bits in
 * a nibble are x or z then uppercase X or Z will be displayed there.
 */
std::string FrSimReg::hexString() {
  //std::string ret("Not yet implemented: 32'hDEADBEEF");
  readSync();
  std::ostringstream rval;
  //rval << widthBits << "'h";
  // Now iterate over nibbles, starting at the most significant nibble, adding
  // one char for each.
  // I could write get_nibble like he did, or I could use RegSlice to access the
  // individual nibbles and to_int to get the values--but I can't get X or Z 
  // that way. Should I add direct access to the vecval? I guess I have it,
  // actually. At least I have it here. So I can create a 4 bit reg that I keep
  // assigning based on a slice of this, then access it's aval and bval to make
  // the next hex output digit.
  // THe problem is that the top nibble is weird. I want to assign the reg based
  // on the 1 or 2 or 3 or 4 bits. So the first assign is a slice from widthBits
  // minus 1 downto the next aligned nibble boundary. Then it becomes easy with
  // a for loop that just subtracts 4 bits every pass until it reaches zero.

  // First assign
  //FrSimReg nib(4,0);
  //int top = widthBits-1;
  //int bot = (top/4)*4; // top and bot may be the same number
  //nib = this->operator()(top,bot); // now nib holds the most significant nibble: FIXME: print that nibble
  // now iterate from bot to zero, subtracting 4 each time
  //for(int i=bot-4; i>0; i-=4) {
  //nib = this->operator()(bot+3, bot);
    // FIXME: print the nibble
  //}
  operator<<(rval, *this);
  return rval.str();
}

/**
 * Return a string that represents the value of the reg in binary format. All
 * bits are represented as 0, 1, x, or z as appropriate.
 */
std::string FrSimReg::binString() {
  readSync();
  //std::string ret("Not yet implemented");
  stringFormat = Bin;
  std::ostringstream rval;
  operator<<(rval, *this);
  return rval.str();
}

/**
 *
 */
std::string FrSimReg::octString() {
  readSync();
  std::string ret("Not yet implemented");
  return ret;
}

/**
 * It's fairly easy to make this work only for 32bit or smaller numbers. It is
 * also possible to make it work in general, but it would be slow.
 * General Algorithm:
 * Make a new reg, larger than this, with value 1. Then loop, multiplying by 10
 * each time until the new reg is larger then this. Then divide by 10 and 
 * subtract the new reg from this as many times as possible. The number of 
 * subtracts at each decade is the number that goes in that position. This is
 * similar to a binary shift and subtract divide, except you can subtract up to
 * nine times since it is base 10. THe shift is the multiply or divide by 10
 * since that's not a natural operation in binary or hex. This won't work if
 * there are any x's or z's in the input number, so check that first and return
 * unknown if there are.
 */
std::string FrSimReg::decString() {
  readSync();
  std::string ret("Not yet implemented");
  return ret;
}


/******************************************************************************/
// Logical Tests

/**
 * Works like the verilog triple equal function.
 * Regs are considered equal as long as every bit position matches, be it 0, 1,
 * X, or Z.
 */
bool FrSimReg::exactlyEqual(const FrSimReg& rhs) {
  readSync();
  rhs.readSync();
  // sizes must match exactly
  // loop through each word in the vecvals, all aval and bval must match exactly
  // The reg widths must be exactly the same as well?
  if(widthBits != rhs.widthBits) return false;
  for(int i=0; i<widthWords; i++) {
    if((vecval[i].aval != rhs.vecval[i].aval) ||
       (vecval[i].bval != rhs.vecval[i].bval)) return false;
  }
  return true;
}

/**
 * Works like the verilog !== function.
 */
bool FrSimReg::exactlyNotEqual(const FrSimReg& rhs) {
  return !exactlyEqual(rhs);
}

/**
 * Works like the verilog == operator. Regs are equal if they have the same
 * value and neither one contains any X or Z values.
 */
bool FrSimReg::operator==(const FrSimReg& rhs) {
  readSync();
  rhs.readSync();
  // sizes must match exactly for now FIXME: allow different sizes as long as
  // larger reg is zero in upper bits. (or sign extended???)
  // it would be better to assume zero padded inputs in the future
  // loop through each word in the vecvals. All aval must match and all bval
  // must be zero
  if(widthBits != rhs.widthBits) return false;
  for(int i=0; i<widthWords; i++) {
    if((vecval[i].aval != rhs.vecval[i].aval) ||
       (0 != vecval[i].bval) || (0 != rhs.vecval[i].bval)) {
//      std::cout << "false comparison" << std::endl;
      return false;
    }
  }
//  std::cout << "true comparison" << std::endl;
  return true;
}

/**
 * Works like the verilog == operator. Regs are equal if they have the same
 * value and neither one contains any X or Z values.
 */
bool FrSimReg::operator==(const int rhs) {
  readSync();
  if(widthWords>1) {
    // make sure upper bits are set to zero
    for(int w=1; w<widthWords; w++) {
      if(vecval[0].aval!=0 || vecval[0].bval!=0) return false;
    }
  }
  if(vecval[0].aval==rhs && vecval[0].bval==0) return true;
  else return false;
}

/**
 * Works like the verilog != operator.
 */
bool FrSimReg::operator!=(const FrSimReg& rhs) {
  return !operator==(rhs);
}

/**
 * Works like the verilog != operator.
 */
bool FrSimReg::operator!=(const int rhs) {
  return !operator==(rhs);
}

/**
 *
 */
bool FrSimReg::operator>(const FrSimReg& rhs) { 
  return !operator<=(rhs);
}

/**
 *
 */
bool FrSimReg::operator<(const FrSimReg& rhs) {
  return !operator>=(rhs);
}

/**
 * Algorithm:
 * Create a copy of this reg that is 1 bit wider then the current one. Subtract
 * rhs from the copy. If the msb is set (negative answer) then result is false.
 * Also return false if the answer contains x's.
 */
bool FrSimReg::operator>=(const FrSimReg& rhs) {
  readSync();
  rhs.readSync();
  return false;
}

/**
 * Create a copy of rhs but wider. Subtract this and check the msb for a neg
 * result. Neg or x result means return false.
 */
bool FrSimReg::operator<=(const FrSimReg& rhs) {
  readSync();
  rhs.readSync();
  return false;
}

/******************************************************************************/

/**
 * Returns x if any bit in the reg is x or z. Otherwise, returns 0 if any bit is
 * 0. Returns one if all bits are one.
 */
FrSimReg FrSimReg::reduce_and() {
  readSync();
  FrSimReg rval(1,0);
  return rval;
}

/**
 * Returns x if any bit in the reg is x or z. Returns 1 if there are an odd
 * number of 1's in the reg. Returns 0 otherwise.
 */
FrSimReg FrSimReg::reduce_xor() {
  readSync();
  FrSimReg rval(1,0);
  return rval;
}

/**
 * Returns x if any bit in the reg is x or z. Otherwise, returns 1 if any bit is
 * 1. Returns zero if all bits are zero.
 */
FrSimReg FrSimReg::reduce_or() {
  readSync();
  FrSimReg rval(1,0);
  return rval;
}

/******************************************************************************/
// Logical and Arithmetic Operations

/**
 * Modify this register by taking the bitwise logical not of it's value.
 * Note that the not of any x or z value is x.
 */
FrSimReg& FrSimReg::logical_not() {
  readSync();
  for(int i=0; i<widthWords; i++) {
    vecval[i].aval = ~vecval[i].aval; // invert the value
    vecval[i].aval |= vecval[i].bval; // set any x or z to x
  }
  clearUnusedBits(vecval, widthWords, widthBits);
  return *this;
}

/**
 * Modify this register by taking the two's compliment of it. This is the same
 * as negating it's value for two's compliment addition and subtraction
 * operations. 
 * Take the logical_not of the reg and add 1.
 */
FrSimReg& FrSimReg::twos_compliment() {
  readSync();
  logical_not();
  //operator+=(1); // FIXME: check it out, but I bet this doesn't do what I want it to!
  // I wanted to add 1, but I bet this adds a 1 bit FrSimReg with value zero!
  // This is probably why I should change the constructor to value, width like
  // he did in teal!
  operator+=(FrSimReg(1, 1)); // this should work
  return *this;
}

/**
 * Bitwise OR in-place.
 * Calculate the bitwise OR between this reg and the rhs argument that is passed
 * in and assign the result back to this reg. 
 * FIXME: doesn't handle x's and z's properly
 * Note that x|x=z|x=z|z=0|x=0|z=x and 1|x=1|z=1
 * FIXME: doesn't handle different sized operands properly
 */
FrSimReg& FrSimReg::operator|=(const FrSimReg& rhs) {
  readSync();
  rhs.readSync();
  for(int i=0; i<widthWords; i++) {
    if(vecval[i].bval==0 && rhs.vecval[i].bval==0) {
      vecval[i].aval = vecval[i].aval | rhs.vecval[i].aval;
    } else {
      // FIXME: this is harsher than required. This sets the output bit to x if
      // any input bit is x or z. Actually, if one input is 1, the output should
      // be 1 even if the other input is x or z. I will fix this later.
      vecval[i].aval |= rhs.vecval[i].aval;
      vecval[i].bval |= rhs.vecval[i].bval;
      vecval[i].aval |= vecval[i].bval; // set z's to x's
    }
  }
  return *this;
}

/**
 * Bitwise AND in-place.
 * FIXME: doesn't handle x's and z's properly
 * Note that x&x=z&x=z&z=1&x=1&z=x and 0&x=0&z=0
 * FIXME: doesn't handle different sized operands properly
 */
FrSimReg& FrSimReg::operator&=(const FrSimReg& rhs) {
  readSync();
  rhs.readSync();
  for(int i=0; i<widthWords; i++) {
    if(vecval[i].bval==0 && rhs.vecval[i].bval==0) {
      vecval[i].aval = vecval[i].aval & rhs.vecval[i].aval;
    } else {
      // FIXME: this is much harsher than required. This is setting a whole 32
      // bit chunk to x if there are any x's or z's anywhere in that chunk in 
      // either operand. Need to analyze all cases.
      vecval[i].aval = 0;
      vecval[i].bval = -1;
    }
  }
  return *this;
}

/**
 * Bitwise XOR in-place.
 * FIXME: doesn't handle x's and z's properly
 * FIXME: doesn't handle different sized operands properly
 */
FrSimReg& FrSimReg::operator^=(const FrSimReg& rhs) {
  readSync();
  rhs.readSync();
  for(int i=0; i<widthWords; i++) {
    if(vecval[i].bval==0 && rhs.vecval[i].bval==0) {
      vecval[i].aval = vecval[i].aval ^ rhs.vecval[i].aval;
    } else {
      // FIXME: see previous two methods
      vecval[i].aval = 0;
      vecval[i].bval = -1;
    }
  }
  return *this;
}

/**
 * Add in-place.
 * Handling X's in an add is somewhat complicated. When any two bits are added,
 * the output is x if either input bit is x or if the carry-in bit is x. The
 * carry-out bit is also x unless two of the three are known to be zero.
 * That algorithm is representitive of real hardware, but it might be better to
 * just set all output bits to x if any input bits are x. I doubt that any
 * design really depends on full proper handling of some bits as unknowns.
 * For now if there are any x's or z' in either input, then the entire output
 * reg will be set to X.
 *
 * FIXME: the problem with implementing this as a += and builing the operator+
 * from that is that the result can never be bigger that the rhs! In other words
 * you can't add two 32 bit regs and put the result in a 33 bit reg. The result
 * will be a overflowed 32 bit reg!
 */
FrSimReg& FrSimReg::operator+=(const FrSimReg& rhs) {
  readSync();
  rhs.readSync();
  // For now, fail if called with a reg larger than 32 bits.
  if(widthBits > 32 || rhs.widthBits > 32) {
    printf("FrSimReg: attempt to add reg wider than 32 bits not supported.\n");
    printf("Aborting...\n");
    exit(1);
  }

  // Check if either input has any bits in bval set and return all x's if so
  // Add the avals, handling the carry across words
  if(vecval[0].bval || rhs.vecval[0].bval) {
    vecval[0].aval = -1; 
    vecval[0].bval = -1;
  } else {
    vecval[0].aval += rhs.vecval[0].aval;
  }
  return *this;
}

/**
 * Subtract in-place.
 * Subtraction has the same X handling issues as addition does.
 * For now if there are any x's or z' in either input, then the entire output
 * reg will be set to X.
 */
FrSimReg& FrSimReg::operator-=(const FrSimReg& rhs) {
  readSync();
  rhs.readSync();
  return *this;
}

/**
 * Right shift in-place.
 */
FrSimReg& FrSimReg::operator>>=(unsigned int rhs) {
  readSync();
  rightShiftVecval(vecval, widthWords, rhs);
  clearUnusedBits(vecval, widthWords, widthBits);
  writeSync();
  return *this;
}

/**
 * Left shift in-place.
 */
FrSimReg& FrSimReg::operator<<=(unsigned int rhs) {
  readSync();
  leftShiftVecval(vecval, widthWords, rhs);
  clearUnusedBits(vecval, widthWords, widthBits);
  writeSync();
  return *this;
}

/**
 *
 */
FrSimReg FrSimReg::operator,(const FrSimReg& rhs) {
  int fullwidth = widthBits + rhs.getWidthBits();
  FrSimReg catReg(fullwidth, rhs);
  catReg(fullwidth-1, rhs.getWidthBits()) = *this;
  return catReg;
}

/******************************************************************************/
// Internal Functions

/**
 *
 */
int FrSimReg::getWidthWordsFromBits(int wbits) const {
  return ((wbits-1)/32)+1;
}

/**
 *
 */
int FrSimReg::allocVecValArray(int words) {
  vecval = new FrSimRegVecVal[words];
  for(int i=0; i<words; i++) {
    vecval[i].aval = 0;
    vecval[i].bval = 0;
  }
  return 0;
}

/**
 * This function shifts a 32 bit int just like the >> operator, but allows the
 * data shifted in to be arbitrary data rather than just zeroes.
 * numShift must be between 1 and 31 inclusive
 * There's really no reason for this to be a class member. It is just a helper.
 */
int rightShift(int data, int carryIn, int numShift) {
  int sdat = data>>numShift;
  int cdat = carryIn<<(32-numShift);
  // right shift always shifts in 1's (??) so we have to mask those out
  int mask = ~(0xffffffff << (32-numShift));
  sdat &= mask;
  return sdat|cdat;
}

/**
 * Shift an entire vecval array in-place
 * wwidth is the number of 32 bit words in the vv array
 * shift must be between 1 and (wwidth*32)-1
 * This function currently assumes that unused bits at the top of the vecval are
 * set to zero. Is it OK to assume this, or should I verify that they are
 * cleared? Also, it is assumed that wordWidth is appropriate for bitWidth.
 * There's really no reason for this to be a class member. It is just a helper.
 */
void rightShiftVecval(FrSimRegVecVal* vv, int wwidth, int shift) {
  // this is inefficient, and should be optimized at somepoint
  for (int shiftIter = 0; shiftIter < shift; shiftIter++) {
    // shift bits one at a time
    for (int vecIter = 0; vecIter < wwidth; vecIter++) {
      if (vecIter == wwidth - 1) {
	// upper word -- shift in 0's
	vv[vecIter].aval = rightShift(vv[vecIter].aval, 0, 1);
	vv[vecIter].bval = rightShift(vv[vecIter].bval, 0, 1);
      } else {
	vv[vecIter].aval = rightShift(vv[vecIter].aval, vv[vecIter+1].aval, 1);
	vv[vecIter].bval = rightShift(vv[vecIter].bval, vv[vecIter+1].bval, 1);
      }
    }
  }
}

/**
 * This function shifts a 32 bits int just like the << operator, but allows the
 * data shifted in to be arbitrary data rather than just zeroes.
 * There's really no reason for this to be a class member. It is just a helper.
 */
int leftShift(int data, int carryIn, int numShift) {
  int sdat = data<<numShift;
  int cdat = carryIn>>(32-numShift);
  // right shift always shifts in 1's (??) so we have to mask those out
  int mask = ~(0xffffffff << numShift);
  cdat &= mask;
  return sdat|cdat;
}

/**
 * This may be more complicated than the rightShiftVecval function above because
 * the most significant bit probably doesn't line up with the msb of the most
 * significant word. Also word shift is different because the msb isn't
 * necessarily on a word boundary. I need to know bitwidth!
 * There's really no reason for this to be a class member. It is just a helper.
 * FIXME: Implement! I started but was thinking right instead of left somehow
 */
void leftShiftVecval(FrSimRegVecVal* vv, int wwidth, int shift) {
  /*
  int bwidth = 0; // FIXME! need this!
  int firstWordBitShift = bwidth%32; // number of bits shift to get first word shift
  int extraShift = shift-firstWordBitShift; // only used if result is positive!
  int wordShift = 1 + extraShift/32; // only if extra shift is positive
  if(extraShift<0) wordShift = 0;
  int bitShift = extraShift%32; // only if extra shift is positive
  if(extraShift<0) bitShift = shift;
  */
  // this is inefficient, and should be optimized at somepoint
  for (int shiftIter = 0; shiftIter < shift; shiftIter++) {
    // shift bits one at a time
    for (int vecIter = (wwidth-1); vecIter >= 0; vecIter--) {
      if (vecIter == 0) {
	// lower word -- shift in 0's
	vv[vecIter].aval = leftShift(vv[vecIter].aval, 0, 1);
	vv[vecIter].bval = leftShift(vv[vecIter].bval, 0, 1);
      } else {
	vv[vecIter].aval = leftShift(vv[vecIter].aval, vv[vecIter-1].aval, 1);
	vv[vecIter].bval = leftShift(vv[vecIter].bval, vv[vecIter-1].bval, 1);
      }
    }
  }
}

/**
 *
 */
void FrSimReg::clearUnusedBits(FrSimRegVecVal* vv, int wwidth, int bwidth) const {
  int maskwidth = ((bwidth-1)%32)+1;
  int bitmask = maskwidth == 32 ? 0xffffffff : ~(0xffffffff<<maskwidth);
  int compww = getWidthWordsFromBits(bwidth);
  if(wwidth!=compww) { // check error case
    // I may want to handle this situation in the future because I might allow
    // resizing an array and not want to realloc the vecval array every time.
    throw 1;
  }
  vv[compww-1].aval &= bitmask;
  vv[compww-1].bval &= bitmask;
}

/**
 *
 */
void FrSimReg::setToX(FrSimRegVecVal* vv, int wwidth, int bwidth) const {
  for(int i=0; i<wwidth; i++) {
    vv[i].aval = ~0;
    vv[i].bval = ~0;
  }
  clearUnusedBits(vv, wwidth, bwidth);
}

/**
 * Its a little weird to call this function const since it clearly changes the
 * value of the reg, but it was necessary to make a lot of other nice operations
 * work. C++ insists on passing a const reference to a FrSimReg for many 
 * operators including some logical comparison, construction and initialization
 * from another simReg. This is fine for the FrSimReg class it's self, but the
 * derived FrSimCReg class really wants the latest values from the simulator for
 * all of those operations. Thus it must be allowed to update the vecval value
 * in an otherwise const class. 
 */
void FrSimReg::setVecvalArray(FrSimRegVecVal *vv) const {
  for(int i=0; i<widthWords; i++) {
    vecval[i].aval = vv[i].aval;
    vecval[i].bval = vv[i].bval;
  }
}





/******************************************************************************/
// FrSimRegSlice Methods

/**
 * Constructor for FrSimRegSlice.
 * This class just wraps a FrSimReg, selecting some subset of the bits. It's never
 * used explicitly in user code, but makes it simple to assign to a slice of a
 * larger reg using a simple expression. The FrSimRegSlice gets created as a temp
 * variable as part of that expression and does it's job without the user ever
 * having to worry.
 * This is a simple helper class so the constructor is trivial.
 */
FrSimRegSlice::FrSimRegSlice(FrSimReg& r, int u, int l)
  :upperBitPos(u), lowerBitPos(l), reg(r) {}

/**
 * unless the rhs becomes const and creates a tmp reg using the const operator()
 * Create a new reg from the regSlice r then copy to this with operator= below
 */
void FrSimRegSlice::operator=(const FrSimRegSlice& r) {// for reg_a(5,2)=reg_b(5,2); 
  FrSimReg val(r);
  operator=(val);
}

/**
 * Overwrite part of reg with r.
 * This operation is very useful to set a field in a larger reg.
 * Possible algorithm:
 * - reg must have more bits then r since it's sliced size is the same as r
 * - create a new reg the size of reg with zero value
 * - copy r into it and shift left by lowerBitPos
 * - use operator |= to or reg and shifted copy of r
 * - must create mask reg first to zero bits to be assigned in reg, otherwise
 *   the or operation won't work right
 * Alt Algorithm:
 * - calc word offsets and do the same thing word wise to avoid a copy
 */
void FrSimRegSlice::operator=(const FrSimReg& r) {// for reg_a(5,2)=reg_b;
  int slicewidth = upperBitPos - lowerBitPos + 1;
  int rhswidth = r.getWidthBits();
  int regwidth = reg.getWidthBits();
  if(rhswidth != slicewidth) printf("FrSimReg::Warning: reg slice assignment size mismatch\n");
  FrSimReg mask(regwidth, 0);
  FrSimReg copy(regwidth, 1);

  mask |= copy;
  for(int i=0; i<slicewidth-1; i++) {
    mask <<= 1;
    mask |= copy;
  }
  mask <<= lowerBitPos; // bit positions to be copied are all 1's in the mask

  // FIXME: the problem here is that x's and z's aren't propagated in the copy!
  // FIXME: only ones and zeroes can be copied!
  // FIXME: I think I need to manipulate the vecvals more directly.
  copy = r;
  copy <<= lowerBitPos;
  copy &= mask; // clears everything in copy except bits to be put in reg

  mask.logical_not(); // bit positions to be copied are all zeroes in the mask
  reg &= mask; // bit positions in reg to be copied into are all zeroes
  reg |= copy;
}

/**
 * Overwrite part of reg with r.
 * This operation is very useful to set a field in a larger reg.
 * Just convert the int to a reg that is exactly the size of the slice and call
 * the operator= for a FrSimReg that is defined above.
 */
void FrSimRegSlice::operator=(const int v) {// for reg_a(5,2)=3
  int slicewidth = upperBitPos - lowerBitPos + 1;
  FrSimReg val(slicewidth, v);
  operator=(val);
}






std::string convertNibToHex(FrSimReg &r) {
  FrSimRegVecVal *vecval = r.getVecvalArray();
  
  //check for X or Z values
  if ( (vecval[0].bval & 0xf) > 0 ) {
    // check for full X or Z
    if ( (vecval[0].bval & 0xf) == 0xf &&
	 (vecval[0].aval & 0xf) == 0x0 )
      return("Z");
    else return("X");
  } else {
    // no X or Z values, just operate on aval
    std::ostringstream retString;
    retString << std::hex << (vecval[0].aval & 0xf);
    return(retString.str());
  }
}

std::ostream& operator<<(std::ostream& out, const FrSimReg& c) {
  if (c.stringHeaderFormat) {
    out << c.widthBits;
    switch(c.stringFormat) {
    case FrSimReg::Bin: out << "'b"; break;
    case FrSimReg::Hex: out << "'h"; break;
    case FrSimReg::Dec: out << "'d"; break;
    case FrSimReg::Oct: out << "'o"; break;
    }
  }
  
  switch(c.stringFormat) {
    // Return a binary formatted string
  case FrSimReg::Bin:
    {
      for (int ArrayIter = c.widthWords - 1; ArrayIter >= 0; ArrayIter--) {
	int topBit;
	if (ArrayIter == c.widthWords - 1) {
	  // top of the array, need to start from remainder bits
	  topBit = (c.widthBits % 32) - 1;
	} else {
	  topBit = 31;
	}
	
	for (int BitIter = topBit; BitIter >= 0; BitIter--) {
	  int myAval = (c.vecval[ArrayIter].aval >> BitIter) & 0x1;
	  int myBval = (c.vecval[ArrayIter].bval >> BitIter) & 0x1;
	  if (myAval == 0 && myBval == 0) out << "0";
	  else if (myAval == 1 && myBval == 0) out << "1";
	  else if (myAval == 0 && myBval == 1) out << "z";
	  else out << "x";
	}
      }
    }
    break;
    // Return a hex formatted string
  case FrSimReg::Hex:
    {
      // Now iterate over nibbles, starting at the most significant nibble, adding
      // one char for each.
      // I could write get_nibble like he did, or I could use RegSlice to access the
      // individual nibbles and to_int to get the values--but I can't get X or Z 
      // that way. Should I add direct access to the vecval? I guess I have it,
      // actually. At least I have it here. So I can create a 4 bit reg that I keep
      // assigning based on a slice of this, then access it's aval and bval to make
      // the next hex output digit.
      // THe problem is that the top nibble is weird. I want to assign the reg based
      // on the 1 or 2 or 3 or 4 bits. So the first assign is a slice from widthBits
      // minus 1 downto the next aligned nibble boundary. Then it becomes easy with
      // a for loop that just subtracts 4 bits every pass until it reaches zero.
      
      // First assign
      FrSimReg nib(4,0);
      int top = c.widthBits-1;
      int bot = (top/4)*4; // top and bot may be the same number
      FrSimReg myNib(c);
      nib = myNib(top,bot);
      out << convertNibToHex(nib);
      
      // now iterate from bot to zero, subtracting 4 each time
      
      for(int i=bot-4; i>=0; i-=4) {
	nib = myNib(i+3, i);
	out << convertNibToHex(nib);
	
      }
    }
    break;
  case FrSimReg::Dec :
    break;
  case FrSimReg::Oct :
    break;
  }
  
  return(out);
}

/**
 * Set a vecval composed of widthWords 32 bits words to the hex string hexStr
 * which is widthChars characters wide.
 */
void setVecvalFromHexString(std::string hexStr, int widthChars, FrSimRegVecVal *vv, int widthWords) {
  for(int chr=0; chr<widthChars; chr++) {
    if(hexStr[chr] == 'x' || hexStr[chr] == 'X') {
      vv[0].aval |= 0x0f;
      vv[0].bval |= 0x0f;
    } else if(hexStr[chr] == 'z' || hexStr[chr] == 'Z') {
      vv[0].aval &= 0xfff0;
      vv[0].bval |= 0x0f;
    } else {
      unsigned int hexDigit;
      std::stringstream hexDigitStream;
      hexDigitStream << hexStr[chr];
      hexDigitStream >> std::hex >> hexDigit;
      vv[0].aval |= hexDigit;
    }
    if(chr<(widthChars-1)) leftShiftVecval(vv, widthWords, 4);
  }
}

/**
 * Evaluate a string literal. Return it's width in bits. Verify that the right 
 * number of characters are supplied for the bitwidth and encoding. Verify that
 * all the characters are legal for the specified encoding.
 *
 * FIXME: just returning width for now. Need to add other checking.
 */
int evalStringLiteral(const std::string &val) {
  std::stringstream ssDataWidth;
  unsigned int strIdx = 0;
  
  // Grab data width
  while (val[strIdx] != '\'') {
    // need to add check for number
    ssDataWidth << val[strIdx++];
    if (strIdx == val.size()) {
      std::cout << "exiting due to invalid value string" << std::endl;
      std::exit(1);
    }
  }

  // now we have data, size and encoding
  int dataWidthBits = atoi(ssDataWidth.str().c_str());
  return dataWidthBits;
}


/**
 *
 */
void FrSimReg::setToString(const std::string &val) {
  std::stringstream ssDataWidth;
  std::stringstream ssDataEncoding;
  std::stringstream ssData;
  
  unsigned int strIdx = 0;
  
  // Grab data width
  while (val[strIdx] != '\'') {
    // need to add check for number
    ssDataWidth << val[strIdx++];
    if (strIdx == val.size()) {
      std::cout << "exiting due to invalid value string" << std::endl;
      std::exit(1);
    }
  }

  // Account for '`'
  strIdx++;
  
  // Get encoding
  ssDataEncoding << val[strIdx++];
    
  // Get data
  while (strIdx < val.size()) {
    if (val[strIdx] != '_') {
      ssData << val[strIdx++];
    } else {
      strIdx++;
    }
  }

  // now we have data, size and encoding
  int dataWidthBits = atoi(ssDataWidth.str().c_str());
  
  // check for decimal encoding larger than 32 bits
  if (ssDataEncoding.str()[0] == 'd' &&
      dataWidthBits > 32) {
    std::cout << "exiting due to decimal encoding > 32 bits" << std::endl;
    std::exit(1);
  }
    
  int dataWidth = getWidthWordsFromBits(dataWidthBits);
  int copySize = (widthWords < dataWidth) ? widthWords : dataWidth;
  
  if (ssDataEncoding.str()[0] == 'b') {
    // binary encoding FIXME: almost certainly broken for reg widths > 32 bits
    for(int i=0; i<copySize; i++) {
      vecval[i].aval = 0;
      vecval[i].bval = 0;
      int bidxStart = 31;
      if ((i+1)*32 > dataWidthBits) {
	bidxStart = (dataWidthBits%32) - 1;
      }
	  	
      for (int bidx = bidxStart; bidx >= 0; bidx--) {
	if (ssData.str()[bidx + (32 * i)] == '1') {
	  int addMe = 1 << (bidxStart - bidx);
	  int added = vecval[i].aval + addMe;
	  vecval[i].aval = added;
	} else if (ssData.str()[bidx + (32 * i)] == 'x' ||
		   ssData.str()[bidx + (32 * 1)] == 'X') {
	  vecval[i].aval = vecval[i].aval | (1 << (bidxStart - bidx));
	  vecval[i].bval = vecval[i].bval | (1 << (bidxStart - bidx));
	} else if (ssData.str()[bidx + (32 * i)] == 'z' ||
		   ssData.str()[bidx + (32 * 1)] == 'Z') {
	  vecval[i].bval = vecval[i].bval | (1 << (bidxStart - bidx));
	}
      }
    }


  } else if (ssDataEncoding.str()[0] == 'd') {
    //decimal encoding
    vecval[0].aval = atoi(ssData.str().c_str());
    vecval[0].bval = 0;


  } else if (ssDataEncoding.str()[0] == 'h') {
    // hex encoding
    //printf("FrSimReg::setToString: assigning hex string %d bits wide: %s\n", dataWidthBits, ssData.str().c_str());
    int hexChars = dataWidthBits/4;

    // FIXME: for now it is still necessary to zero out the vecval before 
    // assigning, but that should be fixed in the setVecval routine
    for (int i = 0; i < copySize; i++) {
      vecval[i].aval = 0;
      vecval[i].bval = 0;
    }
    setVecvalFromHexString(ssData.str(), hexChars, vecval, widthWords);

  } else {
    std::cout << "unknown encoding '" << ssDataEncoding.str() << "'" << std::endl;
    std::exit(1);
  }
  
  for(int i=copySize; i<widthWords; i++) { // set extra words to zero, if any
    vecval[i].aval = 0;
    vecval[i].bval = 0;
  }
  clearUnusedBits(vecval, widthWords, widthBits);
  writeSync();
};






