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

#ifndef FRVMENV_H_
#define FRVMENV_H_

#include "FrSimReport.h"
#include <iostream>
#include <string>

class FrVmEnv
{
private:
	bool genCfgComplete;
	bool buildComplete;
	bool resetDutComplete;
	bool cfgDutComplete;
	bool startComplete;
	bool waitForEndComplete;
	bool stopComplete;
	bool cleanupComplete;
	
	std::string name;
	
public:

	FrVmEnv(const std::string &_name);
	~FrVmEnv();

	FrSimReport rpt;
	
	void GenCfgStep(); 
	virtual void GenCfg() = 0;
	
	void BuildStep();
	virtual void Build() = 0;
	
	void ResetDutStep();
	virtual void ResetDut() = 0;
	
	void CfgDutStep();
	virtual void CfgDut() = 0;
	
	void StartStep();
	virtual void Start() = 0;
	
	void WaitForEndStep();
	virtual void WaitForEnd() = 0;
	
	void StopStep();
	virtual void Stop() = 0;
	
	void CleanupStep();
	virtual void Cleanup() = 0;
	
	void Run();
	
};

#endif /*FRVMENV_H_*/
