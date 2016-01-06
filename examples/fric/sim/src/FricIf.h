#ifndef FRIC_IF_H_
#define FRIC_IF_H_

#include <string>
#include "frsim/FrSimCReg.h"

class FricIf
{
 public:
  std::string name;

  FrSimCReg *clk;
  FrSimCReg *in;
  FrSimCReg *out;

  FricIf(std::string name, std::string path);
};

class FricMonIf
{
 public:
  std::string name;

  FrSimCReg *clk;
  FrSimCReg *in;
  FrSimCReg *out;

  FricMonIf(std::string name, std::string path);
};

#endif // FRIC_IF_H_
