#include "FricXactor.h"

const int FricXactor::RecieveCB = 100;
const int FricXactor::SendCB = 101;

// FIXME: This should probably just take in the FrSimCRegs instead of connecting via 
//        a path
FricXactor::FricXactor(std::string name, FricIf *intf) :
  FrVmXactor(name),
  inChan("inChan", 1),
  outChan("outChan", 1),
  _sif(FrSimIface::getSimIface()),
  _intf(intf)
{
  CreateThreads();
}

FricXactor::~FricXactor()
{
}

void FricXactor::CreateThreads()
{
  FrSimThreadObj<FricXactor> *sendThread = new FrSimThreadObj<FricXactor>(name+"_sendThread", 
									  this,
									  &FricXactor::Send,
									  0);

  ThreadList.push_back(sendThread);

  FrSimThreadObj<FricXactor> *recvThread = new FrSimThreadObj<FricXactor>(name+"_recvThread",
									  this,
									  &FricXactor::Receive,
									  0);

  ThreadList.push_back(recvThread);

}

void *FricXactor::Send(void *args)
{
  for(;;)
    {
      boost::shared_ptr<FricData> sendData = inChan.get();

      ExecuteCallbacks(SendCB, sendData.get());
      _sendPacket(*(sendData->ptype), *(sendData->port), *(sendData->addr), *(sendData->data));
    }

  return 0;
}

void *FricXactor::Receive(void *args)
{
  for(;;)
    {
      boost::shared_ptr<FricData> recvData(new FricData("FricRecieve"));

      _recvPacket(recvData->ptype, recvData->port, recvData->addr, recvData->data);
      
      outChan.put(recvData);
      ExecuteCallbacks(RecieveCB, recvData.get());
    }

  return 0;
}	  

void FricXactor::_sendPacket(FrSimReg &type, FrSimReg &port, FrSimReg &addr, FrSimReg &data) {
  *(_intf->out) = type;
  *(_intf->out) = *(_intf->out) << 4;
  *(_intf->out) |= port;
  _sif->wait(posedge((_intf->clk)));
  *(_intf->out) = addr;
  _sif->wait(posedge((_intf->clk)));
  if(type==2 || type==5) {
    *(_intf->out) = data;
    _sif->wait(posedge((_intf->clk)));
    *(_intf->out) = data>>8;
    _sif->wait(posedge((_intf->clk)));
  }
  *(_intf->out) = 0;
}

void FricXactor::_recvPacket(FrSimReg *type, FrSimReg *port, FrSimReg *addr, FrSimReg *data) {
  int d0 = (_intf->in)->to_int();
  while (d0==0) { // FIXME: allow for a timeout!
    _sif->wait(posedge((_intf->clk)));
    d0 = (_intf->in)->to_int();
  }

  //  printf("Found a packet! d0=%x\n", d0);
  //d0 = fric_in->to_int();

  *type = (d0 >> 4) & 0x0f;
  *port = d0 & 0x0f;
  _sif->wait(posedge((_intf->clk)));
  *addr = (_intf->in)->to_int() & 0xff;
  if(*type==2 || *type==5) {
    _sif->wait(posedge((_intf->clk)));
    *data = (_intf->in)->to_int();
    _sif->wait(posedge((_intf->clk)));
    d0 = (_intf->in)->to_int();
    *data = *data | FrSimReg(16, (d0<<8));
  }
  _sif->wait(posedge((_intf->clk)));

}

