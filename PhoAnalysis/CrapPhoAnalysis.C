// Original Author:  Dongwook Jang
// $Id: ana.C,v 1.11 2013/05/05 12:01:32 yiiyama Exp $

void CrapPhoAnalysis(TString outputName="analysis"){

  // REMOVE THE LINE BELOW IF NOT RUNNING IN CMSSW ENVIRONMENT
  //gSystem->Load("libCondFormatsJetMETObjects.so");

  //gSystem->Load("../../SUSYPhotonAnalysis/SusyNtuplizer/macro/libSusyEvent.so");

  //gSystem->AddIncludePath("-I" + TString(gSystem->Getenv("CMSSW_RELEASE_BASE")) + "/src");

  // Analysis macro
  //gROOT->LoadMacro("SusyEventAnalyzer.cc+");

  //gSystem->AddIncludePath("-I /Users/dmason/play/photons/SUSYPhotonAnalysis/SusyNtuplizer/src");
  //gSystem->AddIncludePath("-I /Users/dmason/play/photons/Crap");

  //gSystem->Load("/Users/dmason/play/photons/SUSYPhotonAnalysis/SusyNtuplizer/src/SusyEvent.h+");
  gROOT->ProcessLine(".L ../SusyEvent.h+");
  gROOT->ProcessLine(".L ../Crap.h+");
  
  gROOT->ProcessLine(".L CrapPhoAnalysis.h+");


  gSystem->Load("../lib/libCrapPhoAnalysis.so");
  
  //gROOT->LoadMacro("CrapPhoAnalysis.cc++");

  // chain of inputs
  TChain chain("susyTree");
  //chain.Add("susyEvents.root");
  chain.Add("root://cmseos.fnal.gov//eos/uscms/store/user/lpcpjm/SusyNtuples/cms538v1/Run2012D-22Jan2013-v1/DoublePhoton/susyEvents_361_1_fC3.root");

  // Disabling unused branches will speed up the processing significantly, but risks inconsistencies if wrong trees are turned off.
  // Make sure you know what you are doing.
  // Especially be careful when producing a skim - disabled branches will not be copied.
  // You can either enable all branches, or have two Event objects (one for event selection with reduced branch configuration, and the
  // other for event copying with branches fully enabled).
  //chain.SetBranchStatus("*", 0);
  //chain.SetBranchStatus("runNumber", 1);
  //chain.SetBranchStatus("luminosityBlockNumber", 1);
  //chain.SetBranchStatus("eventNumber", 1);
  //chain.SetBranchStatus("isRealData", 1);
  //chain.SetBranchStatus("metFilterBit", 1);
  //chain.SetBranchStatus("rho", 1);
  //chain.SetBranchStatus("rho25", 1);
  //chain.SetBranchStatus("hlt*", 1);
  //chain.SetBranchStatus("vertices*", 1);
  //chain.SetBranchStatus("photons_photons*", 1);
  //chain.SetBranchStatus("muons_muons*", 1);
  //chain.SetBranchStatus("electrons_gsfElectrons*", 1);
  //chain.SetBranchStatus("pfJets_ak5*", 1);
  //chain.SetBranchStatus("met_pfType01CorrectedMet*", 1);
  //chain.SetBranchStatus("pfParticles*", 1);

  if(chain.LoadTree(0) != 0){
    cerr << "Error with input chain. Do the files exist?" << endl;
    return;
  }

  CrapPhoAnalysis sea(chain);

  sea.SetOutput(outputName);
  sea.SetLogFile("cout"); // set to a full path to a file to output log to a file
  sea.SetPrintInterval(1000);
  sea.SetPrintLevel(0);
  sea.AddHltName("HLT_Photon36_CaloId10_Iso50_Photon22_CaloId10_Iso50");
  sea.CopyEvents(true);
  sea.SetProcessNEvents(-1);

  TStopwatch ts;

  ts.Start();

  sea.Run();

  ts.Stop();

  std::cout << "RealTime : " << ts.RealTime()/60.0 << " minutes" << std::endl;
  std::cout << "CPUTime  : " << ts.CpuTime()/60.0 << " minutes" << std::endl;

}
