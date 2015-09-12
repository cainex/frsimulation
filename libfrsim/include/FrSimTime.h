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

#ifndef FRSIMTIME_H
#define FRSIMTIME_H


class FrSimTime {
 public:
  // Should have methods to check equal, greater than, less than, as well as do
  // add, subtract, set, and maybe others.
  // This currently looks like a vpi time structure. I guess there's no real
  // need to keep it the same, but it seemed like a reasonable place to start.

  int type;
  uint high;
  uint low;
  double real;
};

#endif

