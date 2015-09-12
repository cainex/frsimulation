#include "FricMonitor.h"
#include <sstream>

const int FricMonitor::InCB = 100;
const int FricMonitor::OutCB = 101;

FricMonitor::FricMonitor(std::string name, FricMonIf *intf) :
  FrVmXactor(name),
  _sif(FrSimIface::getSimIface()),
  _intf(intf),
  _numIn(0),
  _numOut(0)
{
  CreateThreads();
}

FricMonitor::~FricMonitor()
{
}

void FricMonitor::CreateThreads()
{
  FrSimThreadObj<FricMonitor> *inputThread = new FrSimThreadObj<FricMonitor>(name+"_inputThread",
									     this,
									     &FricMonitor::Input,
									     0);

  ThreadList.push_back(inputThread);

  FrSimThreadObj<FricMonitor> *outputThread = new FrSimThreadObj<FricMonitor>(name+"_outputThread",
									     this,
									     &FricMonitor::Output,
									     0);

  ThreadList.push_back(outputThread);

}

void *FricMonitor::Input(void *args)
{
  int d0 = 0;

  for (;;)
    {
      d0 = (_intf->in)->to_int();
      while (d0==0) {
	_sif->wait(posedge(_intf->clk));
	d0 = (_intf->in)->to_int();
      }

      std::stringstream dataName;
      dataName << "InData_" << _numIn++;
      FricData *inData = new FricData(dataName.str());
      *(inData->ptype) = (d0 >> 4) & 0x0f;
      *(inData->port) = d0 & 0x0f;
      _sif->wait(posedge(_intf->clk));
      *(inData->addr) = (_intf->in)->to_int() & 0xff;

      if ( *(inData->ptype)==2 || *(inData->ptype)==5 ) {
	_sif->wait(posedge(_intf->clk));
	*(inData->data) = (_intf->in)->to_int() & 0xff;
	_sif->wait(posedge(_intf->clk));
	d0 = (_intf->in)->to_int() & 0xff;
	*(inData->data) = *(inData->data) | FrSimReg(16, (d0<<8));
      }

      ExecuteCallbacks(InCB, inData);
      delete inData;

      _sif->wait(posedge(_intf->clk));
    }      
  
  return 0;
}

void *FricMonitor::Output(void *args)
{

  int d0 = 0;

  for (;;)
    {
      d0 = (_intf->out)->to_int();
      while (d0==0) {
	_sif->wait(posedge(_intf->clk));
	d0 = (_intf->out)->to_int();
      }

      std::stringstream dataName;
      dataName << "OutData_" << _numOut++;
      FricData *outData = new FricData(dataName.str());
      *(outData->ptype) = (d0 >> 4) & 0x0f;
      *(outData->port) = d0 & 0x0f;
      _sif->wait(posedge(_intf->clk));
      *(outData->addr) = (_intf->out)->to_int() & 0xff;

      if ( *(outData->ptype)==2 || *(outData->ptype)==5 ) {
	_sif->wait(posedge(_intf->clk));
	*(outData->data) = (_intf->out)->to_int() & 0xff;
	_sif->wait(posedge(_intf->clk));
	d0 = (_intf->out)->to_int() & 0xff;
	*(outData->data) = *(outData->data) | FrSimReg(16, (d0<<8));
      }

      ExecuteCallbacks(OutCB, outData);
      delete outData;

      _sif->wait(posedge(_intf->clk));
    }

  return 0;
}

