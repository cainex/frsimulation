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

#include "FrVmEnv.h"

FrVmEnv::FrVmEnv(const std::string &_name) : 
	genCfgComplete(false),
	buildComplete(false),
	resetDutComplete(false),
	cfgDutComplete(false),
	startComplete(false),
	waitForEndComplete(false),
	stopComplete(false),
	cleanupComplete(false),
	name(_name),
	rpt(_name)
{
}

FrVmEnv::~FrVmEnv()
{
}

void FrVmEnv::Run()
{
	if (!cleanupComplete)
		CleanupStep();
}

void FrVmEnv::CleanupStep()
{
	if (!stopComplete)
		StopStep();

	if (!cleanupComplete) {		
		Cleanup();
		cleanupComplete = true;
	}
}

void FrVmEnv::StopStep()
{
	if (!waitForEndComplete)
		WaitForEndStep();
	
	if (!stopComplete) {	
		Stop();
		stopComplete = true;
	}
}

void FrVmEnv::WaitForEndStep()
{
	if (!startComplete)
		StartStep();
	
	if (!waitForEndComplete) {	
		WaitForEnd();
		waitForEndComplete = true;
	}
}

void FrVmEnv::StartStep()
{
	if (!cfgDutComplete)
		CfgDutStep();
	
	if (!startComplete) {	
		Start();
		startComplete = true;
	}
}

void FrVmEnv::CfgDutStep()
{
	if (!resetDutComplete)
		ResetDutStep();
	
	if (!cfgDutComplete) {	
		CfgDut();
		cfgDutComplete = true;
	}
}

void FrVmEnv::ResetDutStep()
{
	if (!buildComplete)
		BuildStep();
		
	if (!resetDutComplete) {
		ResetDut();
		resetDutComplete = true;
	}
}

void FrVmEnv::BuildStep()
{
	if (!genCfgComplete)
		GenCfgStep();
	
	if (!buildComplete) {	
		Build();
		buildComplete = true;
	}
}	

void FrVmEnv::GenCfgStep()
{
	if (!genCfgComplete) {
		GenCfg();
		genCfgComplete = true;
	}
}
