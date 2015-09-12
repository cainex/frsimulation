#include "FrVmXactor.h"
#include "FrVmChannel.h"
#include "FricData.h"
#include "FricSlaveIf.h"

class FricSlaveXactor : public FrVmXactor
{
public:
  static const int SendCB;
  static const int RecieveCB;

  FrVmChannel<FricData> inChan;
  FrVmChannel<FricData> outChan;
  
  FricSlaveXactor(std::string name, FricSlaveIf *slaveIntf);
  ~FricSlaveXactor();
  
  void CreateThreads();
  
  void *Recieve(void *args);

protected:
  FricSlaveIf *_intf;
  FrSimIface *_sif;
  int _regs[256];

  void _recievePacket(FrSimReg &type, FrSimReg &port, FrSimReg &addr, FrSimReg &data);

};

