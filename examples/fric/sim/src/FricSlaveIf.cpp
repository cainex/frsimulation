#include "FricSlaveIf.h"

FricSlaveIf::FricSlaveIf(std::string name, std::string path) :
  name(name),
  clk(new FrSimCReg((path + ".clk").c_str(), 1, 0, 0)),
  rst(new FrSimCReg((path + ".rst").c_str(), 1, 0, 0)),
  addr(new FrSimCReg((path + ".addr").c_str(), 8, 0, 0)),
  wdat(new FrSimCReg((path + ".wdat").c_str(), 16, 0, 0)),
  wstb(new FrSimCReg((path + ".wstb").c_str(), 1, 0, 0)),
  rdat(new FrSimCReg((path + ".rdat").c_str(), 16, 1, 0))
{

}

  

