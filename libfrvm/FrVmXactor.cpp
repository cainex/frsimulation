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

#include "FrVmXactor.h"

FrVmXactor::FrVmXactor(std::string _name) :
	name(_name),
	rpt(FrSimReport(_name))
{
  stt = FrSimThreadTracker::getThreadTracker();
  sif = FrSimIface::getSimIface();
}

FrVmXactor::~FrVmXactor()
{
}

void FrVmXactor::Start()
{
  rpt.Report(FrSimReportControl::DEBUG, "creating main threads");
  for (std::vector<FrSimThread *>::iterator it = ThreadList.begin(); it != ThreadList.end(); it++) {
    (*it)->spawn();
  }
}

void FrVmXactor::RegisterCallback(FrVmCallbackBase *cb)
{
  CallbackList.push_back(cb);
}

void FrVmXactor::ExecuteCallbacks(const int &_id, void *_data)
{
  for (std::vector<FrVmCallbackBase *>::iterator it = CallbackList.begin(); it != CallbackList.end(); it++) {
    (*it)->Callback(_id, _data);
  }
}

