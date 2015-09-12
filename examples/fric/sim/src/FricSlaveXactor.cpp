#include "FricSlaveXactor.h"

const int FricSlaveXactor::SendCB = 101;
const int FricSlaveXactor::RecieveCB = 102;

// FIXME: This should probably just take in the FrSimCRegs instead of connecting via 
//        a path
FricSlaveXactor::FricSlaveXactor(std::string name, FricSlaveIf *masterIntf) :
  FrVmXactor(name),
  inChan("inChan", 100),
  outChan("outChan", 100),
  _intf(masterIntf),
  _sif(FrSimIface::getSimIface())
{
  CreateThreads();
}

FricSlaveXactor::~FricSlaveXactor()
{
}

void FricSlaveXactor::CreateThreads()
{
  FrSimThreadObj<FricSlaveXactor> *recieveThread = new FrSimThreadObj<FricSlaveXactor>(name+"_recvThread", 
										       this,
										       &FricSlaveXactor::Recieve,
										       0);

  ThreadList.push_back(recieveThread);

}

void *FricSlaveXactor::Recieve(void *args)
{
  for(;;)
    {
      boost::shared_ptr<FricData> recvData(new FricData("recvData"));

      _recievePacket(*(recvData->ptype), *(recvData->port), *(recvData->addr), *(recvData->data));
      ExecuteCallbacks(RecieveCB, recvData.get());

      outChan.put(recvData);

    }

  return 0;
}

void FricSlaveXactor::_recievePacket(FrSimReg &type, FrSimReg &port, FrSimReg &addr, FrSimReg &data) {
  for(;;) {
    _sif->wait(posedge(_intf->clk));
    //printf("Running recvXaction!\n");
    if((_intf->wstb->to_int())==1) {
      int addr = _intf->addr->to_int();
      int wdat = _intf->wdat->to_int();
      printf("Wrote %4.4x to %d\n", wdat, addr);
      _regs[_intf->addr->to_int()] = _intf->wdat->to_int();
    }
    *(_intf->rdat) = _regs[_intf->addr->to_int()];
  }
}

