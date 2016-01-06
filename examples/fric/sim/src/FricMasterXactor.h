#include "frvm/FrVmXactor.h"
#include "frvm/FrVmChannel.h"
#include "FricData.h"
#include "FricMasterIf.h"

class FricMasterXactor : public FrVmXactor
{
public:
  static const int SendCB;
  static const int RecieveCB;

  FrVmChannel<FricData> inChan;
  FrVmChannel<FricData> outChan;
  
  FricMasterXactor(std::string name, FricMasterIf *masterIntf);
  ~FricMasterXactor();
  
  void CreateThreads();
  
  void *Send(void *args);

protected:
  FricMasterIf *_intf;
  FrSimIface *_sif;

  void _sendPacket(FrSimReg &type, FrSimReg &port, FrSimReg &addr, FrSimReg &data);

};

