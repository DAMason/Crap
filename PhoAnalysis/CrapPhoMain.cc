// This is here to basically avoid CINT -- move options to command line vars and have an actual executable rather than interpreted root wackiness...

#include <TChain.h>
#include <iostream>

#include "../Crap.h"

// Inherit from Crap
class CrapPhoAnalysis: public Crap {
  public:
  CrapPhoAnalysis(TTree& tree):Crap(tree) {}

  /* Main analyzer function to be defined */
  virtual void Run();
};


int main(int argc, char* argv[]) {

std::cout << "Argc: " << argc << std::endl;

if (argc==0) std::cout << "No Arguments" << std::endl;

for (int iarg=1;iarg<argc && argv[iarg][0] == '-';iarg++) {
  std::cout << "Arg: " << iarg << "  " << argv[iarg] << std::endl;

  }
  
  
TChain chain("susyTree");
chain.Add("root://cmseos.fnal.gov//eos/uscms/store/user/lpcpjm/SusyNtuples/cms538v1/Run2012D-22Jan2013-v1/DoublePhoton/susyEvents_361_1_fC3.root");

TString outputName="analysis";
  
CrapPhoAnalysis sea(chain);

  sea.SetOutput(outputName);
  sea.SetLogFile("cout"); // set to a full path to a file to output log to a file
  sea.SetPrintInterval(1000);
  sea.SetPrintLevel(0);
  sea.AddHltName("HLT_Photon36_CaloId10_Iso50_Photon22_CaloId10_Iso50");
  sea.CopyEvents(true);
  sea.SetProcessNEvents(-1);
  
  sea.Run();



}