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
 * Creation_Date:  Feb 2012
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

#include "FricEnv.h"
#include "FrVmCallback.h"
#include "FrSimReport.h"

#include <sstream>

/******************************************************************************/
class FricResponseCallbacks : public FrVmCallbackBase
{
public:
  FrSimReport *rpt;

  FricResponseCallbacks(std::string name) : rpt(new FrSimReport(name)) {}

  virtual void Callback(const int &_id, void *_data)
  {
    FricData *data = static_cast<FricData *>(_data);

    if (_id == FricResponseGen::TransactionReceived)
      {
	rpt->Report(FrSimReportControl::NOTE, "Transaction Received ::");
	data->display();
      }
    else if (_id == FricResponseGen::SendingResponse)
      {
	rpt->Report(FrSimReportControl::NOTE, "Sending Response ::");
	data->display();
      }
  }
};
/******************************************************************************/

/******************************************************************************/
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
/******************************************************************************/

/******************************************************************************/
class MstrGenCallbacks : public FrVmCallbackBase
{
public:
  FrSimReport *rpt;

  MstrGenCallbacks(std::string name) : rpt(new FrSimReport(name)) {}

  virtual void Callback(const int &_id, void *_data)
  {
    FricMasterGenData *data = static_cast<FricMasterGenData *>(_data);

    if (_id == FricMasterGen::PreRandomizeCB)
      {
	rpt->Report(FrSimReportControl::NOTE, "Randomizing mstr gen data");
      }
    else if (_id == FricMasterGen::PostRandomizeCB)
      {
	rpt->Report(FrSimReportControl::NOTE, "Randomized Data ::");
	data->display();
      }
    else if (_id == FricMasterGen::ResponseCB)
      {
	rpt->Report(FrSimReportControl::NOTE, "Response Recieved ::");
	data->display();
      }
  }

};
/******************************************************************************/

/******************************************************************************/
class FricMonRptCallbacks : public FrVmCallbackBase
{
public:
  FrSimReport *rpt;

  FricMonRptCallbacks(std::string name) : rpt(new FrSimReport(name)) {}

  virtual void Callback(const int &_id, void *_data)
  {
    FricData *data = static_cast<FricData *>(_data);

    if (_id == FricMonitor::InCB)
      {
	rpt->Report(FrSimReportControl::NOTE, "Inbound observed ::");
	data->display();
      }
    else if (_id == FricMonitor::OutCB)
      {
	rpt->Report(FrSimReportControl::NOTE, "Outbound observed ::");
	data->display();
      }
  }
};

/******************************************************************************/


/******************************************************************************/
void* frsim_main(void*) {
  FrSimIface *sif = FrSimIface::getSimIface();
  FrSimCReg clk("tb_top.clk", 1, 0, 0);

  FrSimReport *rpt = new FrSimReport("randTest");

  FricEnv *env = new FricEnv("Fric ENV", &clk);

  env->BuildStep();
  //  int data = 0x00005a0f;

  // env->master->RegisterCallback(new FricRptCallbacks("Fric Master"));

  for (int i = 0; i < 8; i++) {
    std::stringstream nameStr;
    nameStr << "Fric Monitor PX" << i;
    env->pxMon[i]->RegisterCallback(new FricMonRptCallbacks(nameStr.str()));
  //   env->respGen[i]->RegisterCallback(new FricResponseCallbacks(nameStr.str()));
  }
    
  env->mstrGen->RegisterCallback(new MstrGenCallbacks("Mstr Generator"));

  // SlaveCallbacks *slv_cb = new SlaveCallbacks();
  // slave->RegisterCallback(slv_cb);

  rpt->Report(FrSimReportControl::NOTE, "randTest: now starting...");

  env->Run();

  sif->finish();

  return(NULL);
}
/******************************************************************************/
