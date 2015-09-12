#ifndef FRICCHECKER_H_
#define FRICCHECKER_H_

#include <string>
#include <vector>
#include <list>
#include "FrSimReport.h"
#include "FricData.h"

class FricChecker
{
 public:
  FricChecker(std::string name, int numPorts);
  ~FricChecker();

  void ProcessInput(int port, FricData *data);
  void ProcessOutput(int port, FricData *data);

  void EndCheck();

  FrSimReport *rpt;

 protected:
  std::string _name;
  std::vector< std::vector< FricData * > > _expectList;
  std::vector< int > _expectCountList;
};

#endif //FRICCHECKER_H_
