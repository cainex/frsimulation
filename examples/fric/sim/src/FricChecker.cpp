#include "FricChecker.h"

FricChecker::FricChecker(std::string name, int numPorts) :
  rpt(new FrSimReport(name)),
  _name(name),
  _expectList(numPorts),
  _expectCountList(numPorts, 0)
{
}

FricChecker::~FricChecker()
{
}

void FricChecker::ProcessInput(int port, FricData *data)
{
  // Set expect for target port
  FricData *targetExpectData = new FricData(data->name+"_targetExpect",
					    data->ptype->to_int(),
					    port,
					    data->addr->to_int(),
					    data->data->to_int());

  _expectList[data->port->to_int()].push_back(targetExpectData);
  _expectCountList[data->port->to_int()]++;
}

void FricChecker::ProcessOutput(int port, FricData *data)
{
  // Check against expect list
  bool matchFound = false;
  for (std::vector< FricData * >::iterator iter = _expectList[port].begin();
       iter != _expectList[port].end();
       iter++)
    {
      if ( (*iter)->compare(data) ) {
	matchFound = true;
	_expectList[port].erase(iter);
	delete (*iter);
	break;
      }
    }
  
  if (!matchFound) {
    rpt->Report(FrSimReportControl::ERROR, "Unexpected transactions ::");
    data->display();
  }
}

void FricChecker::EndCheck()
{
  bool expectListEmpty = true;

  for ( std::vector< std::vector< FricData * > >::iterator iter = _expectList.begin();
	iter != _expectList.end();
	iter++)
    {
      if ( (*iter).size() != 0) {
	expectListEmpty = false;
	break;
      }
    }

  if (!expectListEmpty) {
    rpt->Report(FrSimReportControl::ERROR, "Expect list not empty!");
  }
  else {
    rpt->Report(FrSimReportControl::NOTE, "Expect list empty");
    for (unsigned int i = 0; i < _expectCountList.size(); i++) {
      std::stringstream expectCountString;
      expectCountString << "Port[" << i << "] saw " << _expectCountList[i] << " transaction";
      rpt->Report(FrSimReportControl::NOTE, expectCountString.str());
    }
  }
  
}



