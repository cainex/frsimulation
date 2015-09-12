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

#include "FrSimReportControl.h"
FrSimReportControl* FrSimReportControl::src = 0;

FrSimReportControl::FrSimReportControl()
{
	globalMaskLevel = NOTE;
	globalErrorThreshold = 1;
	globalErrorCount = 0;
}

FrSimReportControl::~FrSimReportControl()
{
}

FrSimReportControl *FrSimReportControl::getReportControl()
{
	if (src == NULL) {
		src = new FrSimReportControl();
	}
	
	return(src);
}

FrSimReportControl::ReportLevel FrSimReportControl::getGlobalMaskLevel()
{
	return(globalMaskLevel);
}

void FrSimReportControl::setGlobalMaskLevel(const ReportLevel &lvl)
{
	globalMaskLevel = lvl;
}
	
int FrSimReportControl::getGlobalErrorThreshold()
{
	return(globalErrorThreshold);
}

void FrSimReportControl::setGlobalErrorThreshold(const int &thresh)
{
	globalErrorThreshold = thresh;
}
	
int FrSimReportControl::getGlobalErrorCount()
{
	return(globalErrorCount);
}

bool FrSimReportControl::reachedGlobalErrorThreshold()
{
	if (globalErrorCount == globalErrorThreshold) {
		return(true);
	} else {
		return(false);
	}
}

void FrSimReportControl::incrGlobalErrorCount()
{
	globalErrorCount++;
}

bool FrSimReportControl::isMasked(const ReportLevel &lvl)
{
	bool masked;
	
	switch (globalMaskLevel) {
		case DEBUG : 
			masked = false;
			break;
		case NOTE :
			if (lvl == DEBUG) {
				masked = true;
			} else {
				masked = false;
			}
			break;
		case WARNING :
			if (lvl == DEBUG || lvl == NOTE) {
				masked = true;
			} else {
				masked = false;
			}
			break;
		case ERROR :
			if (lvl == DEBUG || lvl == NOTE || lvl == WARNING) {
				masked = true;
			} else {
				masked = false;
			}
			break;
		case FATAL :
			if (lvl == DEBUG || lvl == NOTE || lvl == WARNING || lvl == ERROR) {
				masked = true;
			} else {
				masked = false;
			}
			break;
	}
	
	return(masked);
}
	
std::string FrSimReportControl::getLevelString(const ReportLevel &lvl)
{
	std::string stringVal;
	
	switch(lvl) {
		case DEBUG :
			stringVal = "DEBUG";
			break;
		case NOTE :
			stringVal = "NOTE";
			break;
		case WARNING :
			stringVal = "WARNING";
			break;
		case ERROR :
			stringVal = "ERROR";
			break;
		case FATAL :
			stringVal = "FATAL";
			break;
	}
	
	return(stringVal);
}

