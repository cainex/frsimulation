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
#ifndef FRVMCHANNEL_H_
#define FRVMCHANNEL_H_

#include <list>
#include <string>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include "FrSimThread.h"

template <class TClass> class FrVmChannel
{
private:
	std::list < boost::shared_ptr< TClass > > chan_data;
	unsigned int size;
	std::string name;
	FrSimThreadCondVar *condNotEmpty;
	FrSimThreadCondVar *condNotFull;
	
public:
	FrVmChannel(const std::string &_name, const unsigned int &_size);
	~FrVmChannel();
	
	void put(boost::shared_ptr<TClass> input);
	boost::shared_ptr<TClass> get();
	boost::shared_ptr<TClass> peek();
	
	void reset();
	std::string getName();
		
};

template <class TClass>
FrVmChannel<TClass>::FrVmChannel(const std::string &_name, const unsigned int &_size) :
	size(_size),
	name(_name),
	condNotEmpty(new FrSimThreadCondVar()),
	condNotFull(new FrSimThreadCondVar())
{
}

template <class TClass>	
FrVmChannel<TClass>::~FrVmChannel()
{
	delete condNotFull;
	delete condNotEmpty;
}

template <class TClass>
void FrVmChannel<TClass>::put(boost::shared_ptr<TClass> input)
{
	condNotFull->lock();
	
	if (chan_data.size() == size) {
		condNotFull->wait();
	}
	
	condNotFull->unlock();
	
	condNotEmpty->lock();
	chan_data.push_back(input);
	condNotEmpty->signal(true);
	condNotEmpty->unlock();	
}

template <class TClass>
boost::shared_ptr<TClass> FrVmChannel<TClass>::get()
{
	boost::shared_ptr<TClass> data;
	
	condNotEmpty->lock();
	if (chan_data.empty()) {
		condNotEmpty->wait();
	}
	
	condNotEmpty->unlock();
	
	condNotFull->lock();
	data = chan_data.front();
	chan_data.pop_front();
	condNotFull->signal(true);
	condNotFull->unlock();
	
	return(data);
}

template <class TClass>
boost::shared_ptr<TClass> FrVmChannel<TClass>::peek()
{
	boost::shared_ptr<TClass> data;
	
	condNotEmpty->lock();
	if (chan_data.empty()) {
		condNotEmpty->wait();
	}
	condNotEmpty->unlock();
	
	data = chan_data.front();
	return(data);
}

template <class TClass>	
void FrVmChannel<TClass>::reset()
{
	chan_data.clear();
}

template <class TClass>
std::string FrVmChannel<TClass>::getName()
{
	return(name);
}

#endif /*FRVMCHANNEL_H_*/
