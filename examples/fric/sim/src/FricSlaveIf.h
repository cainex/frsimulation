#ifndef FRIC_SLAVE_IF_H_
#define FRIC_SLAVE_IF_H_

#include <string>
#include "frsim/FrSimCReg.h"

class FricSlaveIf
{
 public:
  std::string name;

  FrSimCReg *clk;
  FrSimCReg *rst;

  FrSimCReg *addr;
  FrSimCReg *wdat;
  FrSimCReg *wstb;
  FrSimCReg *rdat;

  FricSlaveIf(std::string name, std::string path);
};


#endif // FRIC_SLAVE_IF_H_
