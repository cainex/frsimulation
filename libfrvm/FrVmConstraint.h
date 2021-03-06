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

#ifndef FRVMCONSTRAINT_H_
#define FRVMCONSTRAINT_H_

#include <string>
#include "FrSimReport.h"

class FrVmConstraint {
 public:
  FrSimReport *rpt;

  FrVmConstraint(std::string name);
  ~FrVmConstraint();

  virtual bool Evaluate() = 0;

  std::string name;
};

#endif // FRVMCONSTRAINT_H_
