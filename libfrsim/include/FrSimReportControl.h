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
#ifndef FRSIMREPORTCONTROL_H_
#define FRSIMREPORTCONTROL_H_

#include <string>

/**
 * FrSimReportControl
 * A singleton class which is used to control global report
 * setting
 */
class FrSimReportControl
{
public:
	/**
	 * ReportLevel
	 * Enumerated type to control reporting levels
	 * DEBUG - Lowest mask level, used for debug reporting messages
	 * NORMAL - masks DEBUG level
	 * WARNING - masks > NORMAL
	 * ERROR - masks masks > WARNING, causes exit after global error 
	 *         threshold reached
	 * FATAL - non-maskable, masks > ERROR, causes immediate exit
	 */
	enum ReportLevel {DEBUG, NOTE, WARNING, ERROR, FATAL};
	
	static FrSimReportControl *getReportControl();
	
	ReportLevel getGlobalMaskLevel();
	void setGlobalMaskLevel(const ReportLevel &lvl);
	
	int getGlobalErrorThreshold();
	void setGlobalErrorThreshold(const int &thresh);
	bool reachedGlobalErrorThreshold();
	
	int getGlobalErrorCount();
	void incrGlobalErrorCount();
	
	virtual ~FrSimReportControl();
	
	bool isMasked(const ReportLevel &lvl);
	std::string getLevelString(const ReportLevel &lvl);
	
private:
	FrSimReportControl();
	static FrSimReportControl *src;
	ReportLevel globalMaskLevel;
	int globalErrorThreshold;
	int globalErrorCount;
	
};

#endif /*FRSIMREPORTCONTROL_H_*/
