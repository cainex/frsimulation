#include "FricEnv.h"
#include <sstream>

FricEnv::FricEnv(std::string name, FrSimCReg *clk) :
  FrVmEnv(name),
  sif(FrSimIface::getSimIface()),
  clk(clk),
  masterIf(0),
  master(0),
  mstrGen(0),
  pxCheck(0)
{
  for (int i = 0; i < 6; i++) {
    pxIf[i] = 0;
    px[i] = 0;
    respGen[i] = 0;
  }
  for (int i = 0; i < 8; i++) {
    pxMonIf[i] = 0;
    pxMon[i] = 0;
  }
}

FricEnv::~FricEnv()
{
}

void FricEnv::GenCfg()
{
  rpt.Report(FrSimReportControl::NOTE, "GenCfg");
}

void FricEnv::Build()
{
  rpt.Report(FrSimReportControl::NOTE, "Build");
  
  masterIf = new FricMasterIf("FrisMasterInterface", "tb_top.mx");
  master = new FricMasterXactor("FricMaster", masterIf);
  mstrGen = new FricMasterGen("mstrGen", &(master->inChan), &(master->outChan));

  for (int i = 0; i < 6; i++) {
    std::stringstream pathStr;
    std::stringstream nameStr;
    pathStr << "tb_top.px" << i+2;
    nameStr << "px" << i+2 << "_if";
    pxIf[i] = new FricIf(nameStr.str(), pathStr.str());

    nameStr.str("");
    nameStr << "px" << i+2 << "_xactor";
    px[i] = new FricXactor(nameStr.str(), pxIf[i]);

    nameStr.str("");
    nameStr << "respGen_p" << i+2;
    respGen[i] = new FricResponseGen(nameStr.str(), &(px[i]->inChan), &(px[i]->outChan));
  }

  pxCheck = new FricChecker("Fric Checker", 8);
  
  for (int i = 0; i < 8; i++) {
    std::stringstream pathStr;
    std::stringstream nameStr;
    pathStr.str("");
    nameStr.str("");
    pathStr << "tb_top.pxMon" << i;
    nameStr << "pxMon" << i << "_if";
    pxMonIf[i] = new FricMonIf(nameStr.str(), pathStr.str());

    nameStr.str("");
    nameStr << "pxMon" << i;
    pxMon[i] = new FricMonitor(nameStr.str(), pxMonIf[i]);

    nameStr.str("");
    nameStr << "pxMonCheck" << i;
    pxMon[i]->RegisterCallback(new FricCheckerCallback(i, pxCheck));
  }

  
}

void FricEnv::ResetDut()
{
  rpt.Report(FrSimReportControl::NOTE, "ResetDut");

  // Fixed delay at start of sim to get past reset
  for(int i=0; i<10; i++) {
    sif->wait(posedge(clk));
    std::stringstream rptStream;
    rptStream << "Clocking design: clock cycle " <<  i;
    rpt.Report(FrSimReportControl::NOTE, rptStream.str());
  }
  
}

void FricEnv::CfgDut()
{
  rpt.Report(FrSimReportControl::NOTE, "CfgDut");
}

void FricEnv::Start()
{
  rpt.Report(FrSimReportControl::NOTE, "Start");

  master->Start();
  for (int i = 0; i < 6; i++) {
    px[i]->Start();
    respGen[i]->Start();
  }

  for (int i = 0; i < 8; i++) {
    pxMon[i]->Start();
  }

  mstrGen->numTransactions = 50;
  mstrGen->Start();
}

void FricEnv::WaitForEnd()
{
  while(mstrGen->numGenerated < mstrGen->numTransactions) {
    sif->wait(posedge(clk));
  }
  
  std::stringstream numGenString;
  numGenString << "Master Generator generated " << mstrGen->numGenerated << " transactions";
  rpt.Report(FrSimReportControl::NOTE, numGenString.str());
  // Let 200 cycles pass
  for(int i=0; i<200; i++) {sif->wait(posedge(clk));}
}

void FricEnv::Stop()
{
}

void FricEnv::Cleanup()
{
  pxCheck->EndCheck();
}

