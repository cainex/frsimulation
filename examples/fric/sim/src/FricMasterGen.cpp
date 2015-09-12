#include "FricMasterGen.h"
#include "FrVmConstraintRange.h"
#include <sstream>

/*********************************************************************************/
FricMasterGenData_DataCons::FricMasterGenData_DataCons(std::string name, FrSimReg *ptype, FrSimReg *data) :
  FrVmConstraint(name),
  _ptype(ptype),
  _data(data)
{
}

FricMasterGenData_DataCons::~FricMasterGenData_DataCons()
{
}

bool FricMasterGenData_DataCons::Evaluate()
{
  if (*_ptype == 2) {
    FrVmConstraintRange dataCons(name+"_range", _data, 1024*1024, 0);
			
    if (!dataCons.Evaluate()) {
      return false;
    }
  } else {
    *_data = 0;
  }

  return true;

}
/*********************************************************************************/

/*********************************************************************************/
FricMasterGenData::FricMasterGenData(std::string name) :
  FricData(name)
{
  FrVmConstraintRange *typeCons = new FrVmConstraintRange(name+"_type_cons",
							  ptype,
							  3,
							  2);
  AddConstraint(typeCons);

  FrVmConstraintRange *portCons = new FrVmConstraintRange(name+"_port_cons",
  							  port,
  							  7,
  							  2);
  AddConstraint(portCons);

  FricMasterGenData_DataCons *dataCons = new FricMasterGenData_DataCons(name+"_data_cons",
  									ptype,
  									data);
  AddConstraint(dataCons);

  FrVmConstraintRange *addrCons = new FrVmConstraintRange(name+"_addr_cons",
  							  addr,
  							  1024,
  							  1);
  AddConstraint(addrCons);
							  
}

FricMasterGenData::~FricMasterGenData()
{
  // need to clean up constraint list
}
/*********************************************************************************/

/*********************************************************************************/
const int FricMasterGen::PreRandomizeCB = 101;
const int FricMasterGen::PostRandomizeCB = 100;
const int FricMasterGen::ResponseCB = 102;

FricMasterGen::FricMasterGen(std::string name, FrVmChannel<FricData> *outChan, FrVmChannel<FricData> *inChan) :
  FrVmXactor(name),
  numTransactions(1),
  numGenerated(0),
  _outChan(outChan),
  _inChan(inChan)
{
  numTransactions = 1;

  CreateThreads();
}

FricMasterGen::~FricMasterGen()
{
}

void FricMasterGen::CreateThreads()
{
  FrSimThreadObj<FricMasterGen> *generateThread = new FrSimThreadObj<FricMasterGen>(name+"_genThread",
										    this,
										    &FricMasterGen::Generate,
										    0);

  ThreadList.push_back(generateThread);

  FrSimThreadObj<FricMasterGen> *receiveThread = new FrSimThreadObj<FricMasterGen>(name+"_rcvThread",
										   this,
										   &FricMasterGen::ResponseMon,
										   0);

  ThreadList.push_back(receiveThread);
}

void *FricMasterGen::Generate(void *args)
{
  for(numGenerated = 0; numGenerated < numTransactions; numGenerated++)
    {
      std::stringstream nextName;
      nextName << "nextData_" << numGenerated;
      boost::shared_ptr<FricMasterGenData> nextData(new FricMasterGenData(nextName.str()));

      ExecuteCallbacks(PreRandomizeCB, nextData.get());

      if (!nextData->randomize()) {
	rpt.Report(FrSimReportControl::ERROR, "Failed to randomize");
      }

      ExecuteCallbacks(PostRandomizeCB, nextData.get());

      _outChan->put(nextData);
    }

  return 0;
}

void *FricMasterGen::ResponseMon(void *args)
{
  for (;;)
    {
      boost::shared_ptr<FricData> recvData;
      recvData = _inChan->get();

      ExecuteCallbacks(ResponseCB, recvData.get());
    }

  return 0;
}
/*********************************************************************************/
