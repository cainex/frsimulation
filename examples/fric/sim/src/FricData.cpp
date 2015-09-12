#include "FricData.h"
#include <sstream>

std::string FricData::psdisplay()
{

  std::stringstream out_string;

  out_string << "FRIC_DATA: ";
  out_string << "type[" << (*ptype) << "] ";
  out_string << "port[" << (*port) << "] ";
  out_string << "addr[" << (*addr) << "] ";
  out_string << "data[" << (*data) << "]" << std::endl;

  return out_string.str();
}

FrVmData *FricData::allocate()
{
  FricData *dta = new FricData(name);
  return dta;
}

FrVmData *FricData::copy(FrVmData *cpy)
{
  FricData *copyData;

  if (cpy != 0)
    {
      copyData = static_cast<FricData*> (cpy);
    }
  else
    {
      copyData = new FricData(name);
    }

  *(copyData->ptype) = *ptype;
  *(copyData->port) = *port;
  *(copyData->addr) = *addr;
  *(copyData->data) = *data;

  return copyData;

}

bool FricData::compare(FrVmData *frData)
{
  // if (*ptype != data->(*ptype))
  //   {
  //     return false;
  //   }
  // else if (*port != data->(*port))

  FricData *fricData = static_cast<FricData*>(frData);

  if (*port != *(fricData->port))
    {
      return false;
    }
  else if (*addr != *(fricData->addr))
    {
      return false;
    }
  else if (*data != *(fricData->data))
    {
      return false;
    }

  return true;
}

