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

#ifndef FRSIMREG_H
#define FRSIMREG_H
// This class should not depend on any other simulation code.
// It might depend on a common logger and some standard libraries.
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>

// Forward reference
class FrSimRegSlice;

/**
 * This very simple struct is used to contain 4 state logic vectors for FrSimReg.
 * Things are stored in this way for easy interaction with verilog simulators.
 */
struct FrSimRegVecVal {
	/**
	 * aval, bval encoding 
	 * aval  bval  bit
	 * ----  ----  ---
	 *    0     0    0
	 *    1     0    1
	 *    0     1    X
	 *    1     1    Z
	 */
  int aval, bval;
};

/**
 * The Reg is a fundamental type for working with hardware. It is an array of
 * bits that can be any width. It can support two state or four state logic, and
 * probably more if necessary. It supports logical operators, shift, assignment,
 * equality, math, and comparison operators. It supports all operations
 * correctly for four state logic.
 *
 * FIXME: assigning a negative int to a reg should sign extend if the reg is
 * longer than 32 bits.
 */
class FrSimReg {
 public:
  enum Formatting {Bin, Hex, Dec, Oct};
  // Constructors
  // Create a w bit register with (optionally) initial value v
  FrSimReg(int w, int v);
  FrSimReg(int vecSize, FrSimRegVecVal *vec);
  FrSimReg(std::string val);
  FrSimReg(int w, std::string val);
  FrSimReg(const FrSimReg& r);
  FrSimReg(const FrSimRegSlice& r);
  FrSimReg(int w, const FrSimReg& r);
  FrSimReg(int w, const FrSimRegSlice& r);
  virtual ~FrSimReg();

  // Assignments. Always writeSync() this, also possibly readSync() val
  FrSimReg& operator=(const FrSimReg& val);
  FrSimReg& operator=(FrSimReg& val);
  FrSimReg& operator=(const int val);
  FrSimReg& operator=(std::string val);
  FrSimReg& operator=(const FrSimRegSlice& val); // For reg_a = reg_b(5,2), unless the rhs becomes const and ends up using the const operator() (???)

 protected:
  // FIXME: why have these function when I already have operator=() that do the same thing?
  // Remove these functions
  void setValue(FrSimReg &val);
  void setValue(const FrSimReg &val);
 public:

  // Accessors: Always readSync() this and rhs
  FrSimRegSlice operator[](int b);
  FrSimRegSlice operator()(int u, int l);
  //FrSimReg operator()(int u, int l) const; // Create a copy if a slice is taken of a const reg
  int to_int();
  std::string hexString();
  std::string binString();
  std::string octString();
  std::string decString();

  // Operators returning a new value
  // Teal defined a four state enum instead of using a bool; is that better?
  // Would it be useful to support operators which compare a reg to a constant?
  // Is that why teal made the default reg constructor take only the regs value?
  // Will C++ then do the conversion and comparison automatically?
  // They could produce a 1 bit FrSimReg, but that still wouldn't eval to true or 
  // false to allow checking with if statements so that's no good.
  bool exactlyEqual(const FrSimReg& rhs);
  bool exactlyNotEqual(const FrSimReg& rhs);
  // X or Z in the input to any of these will produce a false result
  bool operator==(const FrSimReg& rhs);
  bool operator==(const int rhs);
  bool operator!=(const FrSimReg& rhs);
  bool operator!=(const int rhs);
  bool operator>(const FrSimReg& rhs); // FIXME: depends on <=
  bool operator<(const FrSimReg& rhs); // FIXME: depend on >=
  bool operator>=(const FrSimReg& rhs); // FIXME: not yet implemented
  bool operator<=(const FrSimReg& rhs); // FIXME: not yet implemented
  // generate a new 1 bit reg
  FrSimReg reduce_and(); // FIXME: not imp, can I return a reference???
  FrSimReg reduce_xor(); // FIXME: not imp
  FrSimReg reduce_or(); // FIXME: not imp

  // Operators that modify this reg: call readSync() to start and writeSync()
  FrSimReg& logical_not();
  FrSimReg& twos_compliment();
  FrSimReg& operator|=(const FrSimReg& rhs);
  FrSimReg& operator&=(const FrSimReg& rhs);
  FrSimReg& operator^=(const FrSimReg& rhs);
  FrSimReg& operator+=(const FrSimReg& rhs);
  FrSimReg& operator-=(const FrSimReg& rhs);
  FrSimReg& operator>>=(unsigned int rhs);
  FrSimReg& operator<<=(unsigned int rhs);

  // Generate new reg which is the concatenation of two or more regs
  FrSimReg operator,(const FrSimReg& rhs);

  // Maintenance functions, supporting internal and derived classes, not normally used by the user
  // left public so that FrSimIface can call them without being a friend class
  FrSimRegVecVal *getVecvalArray() const {return(vecval);}
  void setVecvalArray(FrSimRegVecVal* vv) const;
  int const getVecvalArraySize() const {return(widthWords);}
  int const getWidthBits() const { return(widthBits); }
  int const getWidthWords() const { return(widthWords); } // returns the same as getVecvalArraySize(), maybe not used now
  	
  // These are empty in the base class, but filled out in the derived FrSimCReg class
  // FIXME: not called by the user, can't these be protected????
  virtual void readSync() const {}
  virtual void writeSync() const {}

  /**
   * setFormatting
   * Set formatting options for string output in getString() and
   * << operator.
   * @param f Formatting enumeration, bin, hex, oct or dec
   * @param h output string format header - 'b, 'h, 'd, 'o
   */
  void setFormatting(const Formatting &f, const bool &h) { 
    stringHeaderFormat = h;
    stringFormat = f;
  }

  /**
   * getFormat
   * Returns the string format
   * return Enumerated string format
   */
  Formatting getFormat() { return stringFormat; } 

 protected:
  int getWidthWordsFromBits(int wbits) const;
  int allocVecValArray(int words);
  void clearUnusedBits(FrSimRegVecVal* vv, int wwidth, int bwidth) const;
  void setToX(FrSimRegVecVal* vv, int wwidth, int bwidth) const;
  void setToString(const std::string &val);
  
  friend class FrSimRegSlice;
  friend std::ostream& operator<<(std::ostream& os, const FrSimReg& s);

  // These values are made mutable in the teal reg class
  // The reason is that they must be updated to match the simulation value when
  // doing what would otherwise be considered a const operation.
  // Likewise, the value in the vecval can be changed here by some const functions,
  // but since the pointer its self doesn't change, I dont' think it's necessary
  // to declare it mutable. Keep this comment as a warning though.
  FrSimRegVecVal *vecval;
  int widthBits;
  int widthWords;
  
  bool stringHeaderFormat;
  Formatting stringFormat;
};

// Helper functions
int rightShift(int data, int carryIn, int numShift);
void rightShiftVecval(FrSimRegVecVal* vv, int wwidth, int shift);
int leftShift(int data, int carryIn, int numShift);
void leftShiftVecval(FrSimRegVecVal* vv, int wwidth, int shift);
void setVecvalFromHexString(std::string hexStr, int widthChars, FrSimRegVecVal *vv, int widthWords);
int evalStringLiteral(const std::string &val);

// Any other operators can be constructed outside the class based on the copy 
// constructor and the self-modifying operators inside the class. No friend decl
// are necessary and they should be small functions.
inline FrSimReg operator|(const FrSimReg& lhs, const FrSimReg& rhs) { FrSimReg ret(lhs); ret |= rhs; return ret; }
inline FrSimReg operator&(const FrSimReg& lhs, const FrSimReg& rhs) { FrSimReg ret(lhs); ret &= rhs; return ret; }
inline FrSimReg operator^(const FrSimReg& lhs, const FrSimReg& rhs) { FrSimReg ret(lhs); ret ^= rhs; return ret; }
inline FrSimReg operator+(const FrSimReg& lhs, const FrSimReg& rhs) { FrSimReg ret(lhs); ret += rhs; return ret; }
inline FrSimReg operator-(const FrSimReg& lhs, const FrSimReg& rhs) { FrSimReg ret(lhs); ret -= rhs; return ret; }
inline FrSimReg operator>>(const FrSimReg& lhs, const unsigned int rhs) { FrSimReg ret(lhs); ret >>= rhs; return ret; }
inline FrSimReg operator<<(const FrSimReg& lhs, const unsigned int rhs) { FrSimReg ret(lhs); ret <<= rhs; return ret; }
inline FrSimReg operator~(const FrSimReg& rhs) { FrSimReg ret(rhs); ret.logical_not(); return ret; }


/**
 * The RegSlice is a reference to a particular sub-section of a Reg. This is 
 * necessary to have if you want to easily assign to just part of a reg. If the
 * slice operator returned a new reg, then you would only have a copy of the
 * original to assign to and couldn't change a slice of the original.
 * This small class is just a helper for FrSimReg and is thus included with it.
 * It is only used as a temp variable in assignments, so it doesn't support much
 * functionality.
 */
class FrSimRegSlice {
 public:
  void operator=(const FrSimRegSlice& r); // for reg_a(5,2)=reg_b(5,2); unless the rhs becomes const and creates a tmp frreg using the const operator() (???)
  void operator=(const FrSimReg& r); // for reg_a(5,2)=reg_b;
  void operator=(const int v); // for reg_a(5,2)=3
 private:
  friend class FrSimReg;
  FrSimRegSlice(FrSimReg& r, int u, int l);
  int upperBitPos;
  int lowerBitPos;
  // If I'm actually going to modify the underlying reg, shouldn't I use a pointer?
  FrSimReg& reg;
};


/**
 * 
 */
template <int W>
class FrSimVec : public FrSimReg {
public:
  FrSimVec(FrSimRegVecVal *vec) : FrSimReg(W, vec) {}
  FrSimVec(int v=0) : FrSimReg(W, v) {}
  FrSimVec(std::string val) : FrSimReg(W, val) {}
  FrSimVec(const FrSimReg& r) : FrSimReg(W, r) {}

  FrSimVec<W> & operator=(std::string val) { setToString(val); return(*this); }
	
};

#endif

