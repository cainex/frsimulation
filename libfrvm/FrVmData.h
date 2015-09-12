/****************************************************************************
* Copyright (c) 2015 by Daniel Grabowski.
*
* This file is part of FrVm.
* 
* FrVm is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* FrVm is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with FrVm.  If not, see <http://www.gnu.org/licenses/>.
*
* Created by: Daniel Grabowski
*
*****************************************************************************/

#ifndef FRVMDATA_H_
#define FRVMDATA_H_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <sstream>
#include "FrSimReport.h"
#include "FrVmConstraint.h"

class FrVmData
{
 public:
  const std::string name;

 protected:
  
  typedef std::vector<FrVmConstraint *> Constraint;
  typedef std::vector<FrVmConstraint *>::iterator ConstraintIterator;

  FrSimReport *rpt;
  Constraint _constraints;
  
public:
	FrVmData(const std::string &_name) :
		name(_name)
	{
		rpt = new FrSimReport(_name);
	}
	
	/**
	 * psdisplay
	 * Returns a formatted string of the packet contents
	 */
	virtual std::string psdisplay() = 0;
	
	/**
	 * allocate
	 * Used to allocate a new FrVmData handle to the extended data class
	 */
	virtual FrVmData *allocate() = 0;
	
	/**
	 * copy
	 * Copies the current packet to either the provided pointer, or a new pointer
	 */
	virtual FrVmData *copy(FrVmData *cpy) = 0;
	
	/**
	 * compare
	 * Compares this packet to provided data member
	 */
	virtual bool compare(FrVmData *data) = 0;
	
	/**
	 * display
	 * Uses psdisplay to get the display string, then uses report class
	 * to display data
	 */
	void display()
	{
		rpt->Report(FrSimReportControl::NOTE, psdisplay());
	}

	/**
	 * add constraint
	 */
	void AddConstraint(FrVmConstraint *constraint)
	{
	  _constraints.push_back(constraint);
	}

	/**
	 * randomize
	 * Iterates through the list of constraints and evalutates each one.
	 */
	bool randomize()
	{
	  for (ConstraintIterator iter = _constraints.begin();
	       iter != _constraints.end();
	       iter++)
	    {
	      if (!(*iter)->Evaluate()) {
		return false;
	      }
	    }

	  return true;
	}
	
};

#endif /*FRVMDATA_H_*/
