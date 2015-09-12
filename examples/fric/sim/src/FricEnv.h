#ifndef _FRIC_ENV_H_
#define _FRIC_ENV_H_

#include "FrVmEnv.h"
#include "FrSimCReg.h"
#include "FrSimIface.h"

#include "FricXactor.h"
#include "FricMasterIf.h"
#include "FricMasterXactor.h"
#include "FricSlaveIf.h"
#include "FricSlaveXactor.h"
#include "FricMasterGen.h"
#include "FricResponseGen.h"
#include "FrVmCallback.h"
#include "FrSimReport.h"
#include "FricData.h"
#include "FricMonitor.h"
#include "FricChecker.h"


class FricCheckerCallback : public FrVmCallbackBase
{
 public:
  FricCheckerCallback(int port, FricChecker *checker) :
  _port(port),
  _checker(checker)
  {}
      
  ~FricCheckerCallback() {}

  virtual void Callback(const int &_id, void *_data)
  {
    FricData *data = static_cast<FricData *>(_data);

    if (_id == FricMonitor::InCB)
      {
        _checker->ProcessInput(_port, data);
      }
    else if (_id == FricMonitor::OutCB)
      {
	_checker->ProcessOutput(_port, data);
      }
  }

 protected:
  int _port;
  FricChecker *_checker;

};

class FricEnv : public FrVmEnv
{
 public:

  FricEnv(std::string name, FrSimCReg *clk);
  ~FricEnv();

  FrSimIface *sif;
  FrSimCReg *clk;

  FricMasterIf *masterIf;
  FricMasterXactor *master;
  FricMasterGen *mstrGen;

  FricIf *pxIf[6];
  FricXactor *px[6];
  FricResponseGen *respGen[6];

  FricMonIf *pxMonIf[8];
  FricMonitor *pxMon[8];
  
  FricChecker *pxCheck;
  
  virtual void GenCfg();
  virtual void Build();
  virtual void ResetDut();
  virtual void CfgDut();
  virtual void Start();
  virtual void WaitForEnd();
  virtual void Stop();
  virtual void Cleanup();

};

#endif // _FRIC_ENV_H_
