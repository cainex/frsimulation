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

#ifndef FRVMXACTOR_H_
#define FRVMXACTOR_H_

#include <vector>
#include "FrSimReport.h"
#include <string>
#include "FrSimThread.h"
#include "FrVmCallback.h"
#include "FrSimThreadTracker.h"
#include "FrSimIface.h"

class FrVmXactor
{
public:
	FrVmXactor(std::string _name);
	virtual ~FrVmXactor();
  	virtual void CreateThreads() = 0;
  	void Start();
  	void RegisterCallback(FrVmCallbackBase *cb);
  	void ExecuteCallbacks(const int &id, void *data);

protected:
  	FrSimThreadTracker *stt;
  	FrSimIface *sif;
  	std::string name;
  	std::vector<FrVmCallbackBase *> CallbackList;
	std::vector<FrSimThread *> ThreadList;
  	FrSimReport rpt;

};

#endif /*FRVMXACTOR_H_*/
