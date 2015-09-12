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

#ifndef FRVMCALLBACK_H_
#define FRVMCALLBACK_H_

class FrVmCallbackBase
{
public:
  virtual void Callback(const int &_id, void *_data) = 0;
};

template <class TClass> class FrVmCallback : public FrVmCallbackBase
{
 private:
  void (TClass::*fpt)(const int &_id, void *_data);
  TClass* pt2Object;

 public:
  FrVmCallback(TClass *_pt2Object, void(TClass::*_fpt)(const int &, void *))
    {
      pt2Object = _pt2Object;
      fpt = _fpt;
    }

  virtual void Callback(const int &_id, void *_data)
  {
    (*pt2Object.*fpt)(_id, _data);
  }
};

#endif /*FRVMCALLBACK_H_*/
