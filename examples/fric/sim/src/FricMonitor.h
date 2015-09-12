#include "FrVmXactor.h"
#include "FricData.h"
#include "FricIf.h"

class FricMonitor : public FrVmXactor
{
public:
  static const int InCB;
  static const int OutCB;

  FricMonitor(std::string _name, FricMonIf *intf);
  ~FricMonitor();
  
  void CreateThreads();
  
  void *Input(void *args);
  void *Output(void *args);

private:
  FrSimIface *_sif;
  FricMonIf *_intf;

  int _numIn;
  int _numOut;
};

