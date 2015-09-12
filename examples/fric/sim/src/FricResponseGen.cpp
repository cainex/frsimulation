#include "FricResponseGen.h"
#include "FrVmConstraintRange.h"
#include <sstream>

FricResponseGenData::FricResponseGenData(std::string name):
  FricData(name)
{
  FrVmConstraintRange *dataCons = new FrVmConstraintRange("dataCons", data, 1024, 1);

  AddConstraint(dataCons);

}

FricResponseGenData::~FricResponseGenData()
{
}


const int FricResponseGen::TransactionReceived = 100;
const int FricResponseGen::SendingResponse = 101;

FricResponseGen::FricResponseGen(std::string name, FrVmChannel<FricData> *outChan, FrVmChannel<FricData> *inChan) :
  FrVmXactor(name),
  _outChan(outChan),
  _inChan(inChan)
{
  CreateThreads();
}

FricResponseGen::~FricResponseGen()
{
}

void FricResponseGen::CreateThreads()
{
  FrSimThreadObj<FricResponseGen> *responseThread = new FrSimThreadObj<FricResponseGen>(name+"_respThread",
											this,
											&FricResponseGen::GenerateResponse,
											0);

  ThreadList.push_back(responseThread);
}

void *FricResponseGen::GenerateResponse(void *args)
{
  for (;;)
    {
      boost::shared_ptr<FricData> recvData = _inChan->get();
      boost::shared_ptr<FricData> sendData(new FricResponseGenData("sendData"));

      ExecuteCallbacks(TransactionReceived, recvData.get());

      if (*(recvData->ptype) == 2)
	{
	  *(sendData->ptype) = 4;
	  *(sendData->port) = *(recvData->port);
	  *(sendData->addr) = *(recvData->addr);
	  *(sendData->data) = 0;
	}
      else if (*(recvData->ptype) == 3)
	{
	  sendData->randomize();	      
	  *(sendData->ptype) = 5;
	  *(sendData->port) = *(recvData->port);
	  *(sendData->addr) = *(recvData->addr);
	}

      ExecuteCallbacks(SendingResponse, sendData.get());

      _outChan->put(sendData);
    }

  return 0;
}
