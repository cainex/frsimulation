#include "FricMasterXactor.h"

const int FricMasterXactor::SendCB = 101;
const int FricMasterXactor::RecieveCB = 100;

// FIXME: This should probably just take in the FrSimCRegs instead of connecting via 
//        a path
FricMasterXactor::FricMasterXactor(std::string name, FricMasterIf *masterIntf) :
  FrVmXactor(name),
  inChan("inChan", 1),
  outChan("outChan", 1),
  _intf(masterIntf),
  _sif(FrSimIface::getSimIface())
{
  CreateThreads();
}

FricMasterXactor::~FricMasterXactor()
{
}

void FricMasterXactor::CreateThreads()
{
  FrSimThreadObj<FricMasterXactor> *sendThread = new FrSimThreadObj<FricMasterXactor>(name+"_sendThread", 
									  this,
									  &FricMasterXactor::Send,
									  0);

  ThreadList.push_back(sendThread);

}

void *FricMasterXactor::Send(void *args)
{
  for(;;)
    {
      boost::shared_ptr<FricData> sendData = inChan.get();

      ExecuteCallbacks(SendCB, sendData.get());
      _sendPacket(*(sendData->ptype), *(sendData->port), *(sendData->addr), *(sendData->data));
      ExecuteCallbacks(RecieveCB, sendData.get());

      outChan.put(sendData);

    }

  return 0;
}

void FricMasterXactor::_sendPacket(FrSimReg &type, FrSimReg &port, FrSimReg &addr, FrSimReg &data) {
  *(_intf->type) = type;
  *(_intf->port) = port;
  *(_intf->addr) = addr;
  *(_intf->wdat) = data;
  *(_intf->tstb) = 1;
  sif->wait(posedge(_intf->clk));
  *(_intf->tstb) = 0;
  while(*(_intf->trdy)!=FrSimReg(1,1)) {
    sif->wait(posedge(_intf->clk));
  }
  if(type==3) {
    while(*(_intf->rstb)!=FrSimReg(1,1)) {
      sif->wait(posedge(_intf->clk));
    }
    data = _intf->rdat->to_int();
  }
}

