#ifndef FRICMASTERGEN_H_
#define FRICMASTERGEN_H_

#include "FrVmXactor.h"
#include "FrVmChannel.h"
#include "FricData.h"

class FricMasterGenData_DataCons : public FrVmConstraint
{
 public:
  FricMasterGenData_DataCons(std::string name, FrSimReg *ptype, FrSimReg *data);
  ~FricMasterGenData_DataCons();

  virtual bool Evaluate();

 protected:
  FrSimReg *_ptype;
  FrSimReg *_data;
};

class FricMasterGenData : public FricData
{
 public:
  FricMasterGenData(std::string name);
  ~FricMasterGenData();
};

class FricMasterGen : public FrVmXactor
{
 public:
  static const int PreRandomizeCB;
  static const int PostRandomizeCB;
  static const int ResponseCB;

  FricMasterGen(std::string name, FrVmChannel<FricData> *outChan, FrVmChannel<FricData> *inChan);
  ~FricMasterGen();

  void CreateThreads();

  void *Generate(void *args);
  void *ResponseMon(void *args);

  int numTransactions;
  int numGenerated;

 protected:
  FrVmChannel<FricData> *_outChan;
  FrVmChannel<FricData> *_inChan;


};

  
#endif // FRICMASTERGEN_H_
