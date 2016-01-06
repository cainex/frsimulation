#include "frvm/FrVmXactor.h"
#include "frvm/FrVmChannel.h"
#include "FricData.h"
#include "FricIf.h"

class FricXactor : public FrVmXactor
{
public:
  static const int RecieveCB;
  static const int SendCB;

  FrVmChannel<FricData> inChan;
  FrVmChannel<FricData> outChan;
  
  FricXactor(std::string name, FricIf *intf);
  ~FricXactor();
  
  void CreateThreads();
  
  void *Send(void *args);
  void *Receive(void *args);

protected:
  FrSimIface *_sif;
  FricIf *_intf;

  void _sendPacket(FrSimReg &type, FrSimReg &port, FrSimReg &addr, FrSimReg &data);
  void _recvPacket(FrSimReg *type, FrSimReg *port, FrSimReg *addr, FrSimReg *data);

};

