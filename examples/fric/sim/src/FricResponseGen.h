#ifndef FRICRESPONSEGEN_H_
#define FRICRESPONSEGEN_H_

#include <string>
#include "FricData.h"
#include "frvm/FrVmChannel.h"
#include "frvm/FrVmXactor.h"

class FricResponseGenData : public FricData
{
 public:
  FricResponseGenData(std::string name);
  ~FricResponseGenData();
};

class FricResponseGen : public FrVmXactor
{
 public:
  static const int TransactionReceived;
  static const int SendingResponse;

  FricResponseGen(std::string name, FrVmChannel<FricData> *outChan, FrVmChannel<FricData> *inChan);
  ~FricResponseGen();

  void CreateThreads();

  void *GenerateResponse(void *args);

 protected:
  FrVmChannel<FricData> *_outChan;
  FrVmChannel<FricData> *_inChan;
};
  

#endif // FRICRESPONSEGEN_H_
