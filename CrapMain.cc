// This is here to basically avoid CINT -- move options to command line vars and have an actual executable rather than interpreted root wackiness...

#include <TChain.h>
#include <iostream>

#include "Crap.h"
#include "PhoAnalysis/CrapPhoAnalysis.h"

using namespace std;

int main(int argc, char* argv[]) {


  int PrintInterval(1000);
  int PrintLevel(0);
  int ProcessNEvents(-1);
  bool CopyEvents=false;
  string OutputBase="analysis";
  string HLTList="";
  string LogFile="cout";
  string InputFileFile="";  // should be initialized empty
  string InputFileList="";  // also should be initialized empty
  
  vector<string> InputFiles;


  cout << "Argc: " << argc << endl;
  if (argc==0) std::cout << "No Arguments" << std::endl;

  //for (int iarg=1;iarg<argc && argv[iarg][0] == '-';iarg++) {
  //for (int iarg=1;iarg<argc;iarg++) {
  int iarg=1;
  while (iarg<argc) {
    cout << "Arg: " << iarg << "  " << argv[iarg] << endl;

    
    if (argv[iarg][0] == '-') {
    
      string argvRemnants=argv[iarg];
      argvRemnants=argvRemnants.substr(1); // stripping off the "-"
      cout << argvRemnants << endl;
       
      if (argvRemnants == "printinterval" ) {
        if (++iarg <=argc) { // has a second argument
          cout << "-printinterval " << argv[iarg] << endl;
          PrintInterval=atoi(argv[iarg]);
          cout << "Setting Print Interval to " << PrintInterval << endl;
        } 
      }
      
      else if (argvRemnants == "printlevel" ) {
        if (++iarg <=argc) { // has a second argument
          cout << "-logfile " << argv[iarg] << endl;
          LogFile=argv[iarg];
          cout << "Setting Log File to " << LogFile << endl;
        } 
      }
      
     else if (argvRemnants == "inputfilefile" ) {
        if (++iarg <=argc) { // has a second argument
          cout << "-inputfilefile " << argv[iarg] << endl;
          InputFileFile=argv[iarg];
          cout << "Reading list of input files from " << InputFileFile << endl;
        } 
      }
      
     else if (argvRemnants == "inputfilelist" ) {
       cout << "-inputfilelist ";
       while (++iarg <=argc && argv[iarg][0] != '-') { // has more arguments and isn't at the end
         
         cout << argv[iarg] << " ";
         InputFiles.push_back(argv[iarg]);
         //cout << "Reading these files: " << InputFileFile << endl;
       }
       cout << endl;
       if (iarg < argc) iarg--;  // Backing up since we probably just hit another argument...
     }
      
     else if (argvRemnants == "logfile" ) {
        if (++iarg <=argc) { // has a second argument
          cout << "-printlevel " << argv[iarg] << endl;
          PrintLevel=atoi(argv[iarg]);
          cout << "Setting Print Level to " << PrintLevel << endl;
        } 
      }
      
     else if (argvRemnants == "processevents" ) {
        if (++iarg <=argc) { // has a second argument
          cout << "-processevents " << argv[iarg] << endl;
          ProcessNEvents=atoi(argv[iarg]);
          cout << "Processing " << ProcessNEvents << " events..." << endl;
        } 
      }
      
      else if (argvRemnants == "outputname" ) {
        if (++iarg <=argc) { // has a second argument
          cout << "-outputname " << argv[iarg] << endl;
          OutputBase=argv[iarg];
          cout << "Output base set to " << OutputBase << endl;
        } 
      }
      
      else if (argvRemnants == "logfile" ) {
        if (++iarg <=argc) { // has a second argument
          cout << "-logfile " << argv[iarg] << endl;
          LogFile=argv[iarg];
          cout << "LogFile set to " << LogFile << endl;
        } 
      }

      else if (argvRemnants == "copyevents") {
        CopyEvents=true;
        cout << "Activating Copy Events..." << endl;       
      }
      
      else {
        cout << "-" << argvRemnants << " isn't recognized..." << endl;  
      }
      
    } else {
      cout << "Something is wonky with the command line args..." << endl;
      cout << "Usage: " << endl;
      cout << "CrapExe  " << endl;
      cout << "                -outputname    <output file base> " << endl;
      cout << "                -copyevents    (no argument -- bool)" << endl;
      cout << "                -printinterval <int # events>" << endl;
      cout << "                -printlevel    <int # 0-2> " << endl;
      cout << "                -addHLTlist    <file> " << endl;
      cout << "                -logfile       <file> " << endl;
      cout << "                -inputfiles    <file> " << endl;
      cout << "                -inputfilelist <file> <file> <file> ..." << endl;
      cout << "                -includejson   <file>  (can be used repeatedly)" << endl;            
    }  
    
    iarg++;
  } // end while argc
  
  
  TChain chain("susyTree");
  string fileline;

  int filecounter(0);
  
  ifstream infilelist;
  infilelist.open(InputFileFile.c_str());
  if (!infilelist.is_open()) {
    cerr << "-inputfilelist specified, but not found!  Hoping there was something in the command line then..." << endl;
  } else {
    while (!infilelist.eof()) {
      getline(infilelist,fileline);
      if (fileline.find("root")!=-1) {
        cout << "File input: " << fileline.c_str() << endl;
        //InputFiles.push_back(fileline);
        chain.Add(fileline.c_str());
        filecounter++;
      }
    }
    infilelist.close();
  }
  
  if (InputFiles.size()>0) {
    for (int ifile=0;ifile<InputFiles.size();++ifile) {
      if (InputFiles[ifile].find("root")!=1) {
        cout << "CL File input: " << InputFiles[ifile].c_str() << endl;
        chain.Add(InputFiles[ifile].c_str());
        filecounter++;
      }
    }
    
  }
  
  if (filecounter==0) {
    //cerr << "Not able to find input file list: " << InputFileFile << endl;
    cerr << "We ended up not adding any files to the chain..." << endl;
    cerr << "Probably not going to do a whole lot here..." << endl;
  }
  

  
  //chain.Add("root://cmseos.fnal.gov//eos/uscms/store/user/lpcpjm/SusyNtuples/cms538v1/Run2012D-22Jan2013-v1/DoublePhoton/susyEvents_361_1_fC3.root");

  //TString outputName="analysis";
  
  CrapPhoAnalysis sea(chain);

  sea.SetOutput(OutputBase);
  sea.SetLogFile(LogFile); // set to a full path to a file to output log to a file
  sea.SetPrintInterval(PrintInterval);
  sea.SetPrintLevel(PrintLevel);
  // we want to get rid of this one I think...  Why is it here?
  sea.AddHltName("HLT_Photon36_CaloId10_Iso50_Photon22_CaloId10_Iso50");
  sea.CopyEvents(CopyEvents);
  sea.SetProcessNEvents(ProcessNEvents);
  
  sea.Run();



}