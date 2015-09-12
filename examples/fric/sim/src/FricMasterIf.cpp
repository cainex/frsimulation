#include "FricMasterIf.h"

FricMasterIf::FricMasterIf(std::string name, std::string path) :
  name(name)
{
  clk = new FrSimCReg((path + ".clk").c_str(), 1, 0, 0);
  rst = new FrSimCReg((path + ".rst").c_str(), 1, 0, 0);

  type = new FrSimCReg((path + ".master_type_o").c_str(), 4, 1, 0);
  port = new FrSimCReg((path + ".master_port_o").c_str(), 4, 1, 0);
  addr = new FrSimCReg((path + ".master_addr_o").c_str(), 8, 1, 0);
  wdat = new FrSimCReg((path + ".master_wdat_o").c_str(), 16, 1, 0);
  tstb = new FrSimCReg((path + ".master_tstb_o").c_str(), 1, 1, 0);
  trdy = new FrSimCReg((path + ".master_trdy").c_str(), 1, 0, 0);
  rstb = new FrSimCReg((path + ".master_rstb").c_str(), 1, 0, 0);
  rdat = new FrSimCReg((path + ".master_rdat").c_str(), 16, 0, 0);
  
}

