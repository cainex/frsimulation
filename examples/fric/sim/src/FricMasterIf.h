#ifndef FRIC_MASTER_IF_H_
#define FRIC_MASTER_IF_H_

#include <string>
#include "FrSimCReg.h"

class FricMasterIf
{
 public:
  std::string name;

  FrSimCReg *clk;
  FrSimCReg *rst;
  FrSimCReg *type;
  FrSimCReg *port;
  FrSimCReg *addr;
  FrSimCReg *wdat;
  FrSimCReg *tstb;
  FrSimCReg *trdy;
  FrSimCReg *rstb;
  FrSimCReg *rdat;

  FricMasterIf(std::string name, std::string path);
};


#endif //FRIC_MASTER_IF_H_
