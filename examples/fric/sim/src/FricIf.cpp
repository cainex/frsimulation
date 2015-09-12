#include "FricIf.h"

FricIf::FricIf(std::string name, std::string path) :
  name(name)
{
  clk = new FrSimCReg((path + ".clk").c_str(), 1, 0, 0);
  in = new FrSimCReg((path + ".fric_in_i").c_str(), 8, 0, 0);
  out = new FrSimCReg((path + ".fric_out_o").c_str(), 8, 1, 0);
}

FricMonIf::FricMonIf(std::string name, std::string path) :
  name(name)
{
  clk = new FrSimCReg((path + ".clk").c_str(), 1, 0, 0);
  in = new FrSimCReg((path + ".fric_in_i").c_str(), 8, 0, 0);
  out = new FrSimCReg((path + ".fric_out_i").c_str(), 8, 0, 0);
}
