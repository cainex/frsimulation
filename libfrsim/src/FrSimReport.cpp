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

#include "FrSimReport.h"

FrSimReport::FrSimReport(std::string _name) :
	name(_name)
{
	src = FrSimReportControl::getReportControl();
	sif = FrSimIface::getSimIface();
}

FrSimReport::~FrSimReport()
{
}

void FrSimReport::Report(const FrSimReportControl::ReportLevel &lvl, const std::string &msg)
{
	if (!src->isMasked(lvl)) {
		if (lvl == FrSimReportControl::WARNING ||
			lvl == FrSimReportControl::ERROR ||
			lvl == FrSimReportControl::FATAL) {
			
			std::cout << "**" << src->getLevelString(lvl) << "**";
		}
		std::cout << "[" << sif->getTime() << "]" << name << " :: " << msg << std::endl;
		if (lvl == FrSimReportControl::ERROR) {
			src->incrGlobalErrorCount();
		}
		
		if (lvl == FrSimReportControl::FATAL ||
			src->reachedGlobalErrorThreshold()) {
		
			std::exit(1);
		}
	}
}

