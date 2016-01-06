/****************************************************************************
 * Copyright (c) 2011 by Focus Robotics. All rights reserved.
 *
 * This program is an unpublished work fully protected by the United States 
 * copyright laws and is considered a trade secret belonging to the copyright
 * holder. No part of this design may be reproduced stored in a retrieval 
 * system, or transmitted, in any form or by any means, electronic, 
 * mechanical, photocopying, recording, or otherwise, without prior written 
 * permission of Focus Robotics, Inc.
 *
 * Proprietary and Confidential
 *
 * Created By   :  Daniel Grabowski
 * Creation_Date:  Jul 11 2011
 * 
 * Brief Description:
 * 
 * Functionality:
 * 
 * Issues:
 * 
 * Limitations:
 * 
 * Testing:
 * 
 ******************************************************************************/

#ifndef FRICDATA_H
#define FRICDATA_H

#include "frsim/FrSimReg.h"
#include "frsim/FrSimReport.h"
#include "frvm/FrVmData.h"

class FricData : public FrVmData {
public:
  FrSimReg *ptype;
  FrSimReg *port;
  FrSimReg *addr;
  FrSimReg *data;

 FricData(const std::string &_name) :
    FrVmData(_name),
    ptype(new FrSimReg(4,0)),
    port(new FrSimReg(4,0)),
    addr(new FrSimReg(8,0)),
    data(new FrSimReg(16,0))
  {
  }

  FricData(const std::string &_name, int _ptype, int _port, int _addr, int _data) :
	 FrVmData(_name),
	 ptype(new FrSimReg(4,_ptype)),
	 port(new FrSimReg(4,_port)),
	 addr(new FrSimReg(8,_addr)),
	 data(new FrSimReg(16,_data))
  {
  }

  virtual ~FricData()
  {
    delete ptype;
    delete port;
    delete addr;
    delete data;
  }

  virtual std::string psdisplay();
  virtual FrVmData *allocate();
  virtual FrVmData *copy(FrVmData *cpy);
  virtual bool compare(FrVmData *data);

};
    
#endif
