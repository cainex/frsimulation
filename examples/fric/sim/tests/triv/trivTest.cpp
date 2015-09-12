/****************************************************************************
 * Copyright (c) 2009 by Focus Robotics. All rights reserved.
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
 * Creation_Date:  Fri Apr  3 2009
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
#include <stdio.h>
#include "FrSimIface.h"
#include "FricXactor.h"
#include "FricMasterIf.h"
#include "FricMasterXactor.h"
#include "FricSlaveIf.h"
#include "FricSlaveXactor.h"
#include "FrVmCallback.h"
#include "FrSimReport.h"
#include "FricData.h"

#include <sstream>

class FricRptCallbacks : public FrVmCallbackBase
{
public:

  FrSimReport *rpt;

  FricRptCallbacks(std::string name) : rpt(new FrSimReport(name)) {}

  virtual void Callback(const int &_id, void *_data)
  {

    FricData *data = static_cast<FricData *>(_data);

    if (_id == FricMasterXactor::SendCB)
      {
	rpt->Report(FrSimReportControl::NOTE, "Transmitting Data ::");
	data->display();
      }
    else if (_id == FricMasterXactor::RecieveCB)
      {
	rpt->Report(FrSimReportControl::NOTE, "Received Data ::");
	data->display();
      }
  }

};


void* frsim_main(void*) {
  FrSimIface *sif = FrSimIface::getSimIface();
  FrSimCReg clk("tb_top.clk", 1, 0, 0);
  FricMasterIf *mIntf = new FricMasterIf("FrisMasterInterface", "tb_top.mx");
  FricMasterXactor *master = new FricMasterXactor("FricMaster", mIntf);
  // FricClientXactor *master = new FricClientXactor("tb_top.mx");
  // FricClientXactor *slave = new FricClientXactor("tb_top.sx");
  FricIf *p2xIf = new FricIf("p2X_if", "tb_top.p2x");
  FricXactor *p2x = new FricXactor("p2x_xactor", p2xIf);
  FricIf *p3xIf = new FricIf("p3X_if", "tb_top.p3x");
  FricXactor *p3x = new FricXactor("p3x_xactor", p3xIf);
  //  int data = 0x00005a0f;

  FricRptCallbacks *mstr_cb = new FricRptCallbacks("Fric Master");
  master->RegisterCallback(mstr_cb);

  FricRptCallbacks *p2x_cb = new FricRptCallbacks("Fric P2x");
  p2x->RegisterCallback(p2x_cb);

  FricRptCallbacks *p3x_cb = new FricRptCallbacks("Fric P3x");
  p3x->RegisterCallback(p3x_cb);

  // SlaveCallbacks *slv_cb = new SlaveCallbacks();
  // slave->RegisterCallback(slv_cb);

  printf("trivTest: now starting...\n");

  // Fixed delay at start of sim to get past reset
  for(int i=0; i<10; i++) {
    sif->wait(posedge(&clk));
    printf("Clocking design: clock cycle %d\n", i);
  }

  master->Start();
  p2x->Start();
  p3x->Start();

  boost::shared_ptr<FricData> sendData(new FricData("sendData", 2, 3, 100, 0)) ;
  boost::shared_ptr<FricData> recvData;
  boost::shared_ptr<FricData> responseData(new FricData("response", 4, 0, 100, 0));

  master->inChan.put(sendData);
  recvData = p3x->outChan.get();
  p3x->inChan.put(responseData);

  boost::shared_ptr<FricData> sendData1(new FricData("sendData1", 3, 3, 100, 0)) ;
  boost::shared_ptr<FricData> recvData1;
  boost::shared_ptr<FricData> responseData1(new FricData("response1", 5, 0, 100, 123));

  master->inChan.put(sendData1);
  recvData = p3x->outChan.get();
  p3x->inChan.put(responseData1);
  
  // Let 200 cycles pass
  for(int i=0; i<200; i++) {sif->wait(posedge(&clk));}

  sif->finish();

  return(NULL);
}

