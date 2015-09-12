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

#ifndef _FRVMCONSTRAINTRANGE_H_
#define _FRVMCONSTRAINTRANGE_H_

#include <string>
#include "FrVmConstraint.h"
#include "FrSimReg.h"

class FrVmConstraintRange : public FrVmConstraint {
 public:
  FrVmConstraintRange(std::string name, FrSimReg *data, int upper, int lower);
  ~FrVmConstraintRange();

  virtual bool Evaluate();

 protected:
  FrSimReg *_data;
  int _upper;
  int _lower;
};

#endif // FRVMCONSTRAINTRANGE_H_
