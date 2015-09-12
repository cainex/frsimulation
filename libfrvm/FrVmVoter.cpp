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

#include "FrVmVoter.h"
#include "FrVmConsensus.h"

FrVmVoter::FrVmVoter(std::string name) :
  name(name)
{
}

FrVmVoter::~FrVmVoter()
{
}

void FrVmVoter::Consent()
{
  _state = CONSENT;
  // if (_consensus != 0)
  //   _consensus->consentEV->trigger();
}

void FrVmVoter::Oppose()
{
  _state = OPPOSE;
  // if(_consensus != 0)
  //   _consensus->consentEV->trigger();
}

void FrVmVoter::Force()
{
  _state = FORCE;
  // if (_consensus != 0)
  //   _consensus->consentEV->trigger();
}


void FrVmVoter::_AddConsensus(FrVmConsensus *consensus)
{
  // if (consensus !=0)
  //   {
  //     _consensus->Remove(this);
  //   }

  // _consensus = consensus;
}

