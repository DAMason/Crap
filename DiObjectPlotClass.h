//
//  DiObjectPlotClass.h
//  CrapProject
//
//  Created by David Mason on 8/29/13.
//  Copyright (c) 2013 David Mason. All rights reserved.
//

#ifndef __CrapProject__DiObjectPlotClass__
#define __CrapProject__DiObjectPlotClass__

#include <iostream>
#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>
#include <TLorentzVector.h>

#endif /* defined(__CrapProject__DiObjectPlotClass__) */

using namespace std;

class DiObjectPlotClass {

public:
  DiObjectPlotClass(TFile*, string);
  virtual ~DiObjectPlotClass();
  void BookHists();
  void Fill(TLorentzVector,TLorentzVector,float);
  void Fill(TLorentzVector,TLorentzVector);

  // Member data is for the most part just a bunch of histograms

  
  TH1F* h_diPt;
  TH1F* h_InvMass;
  TH1F* h_dPhi;
  TH1F* h_dR;
  
  TH2F* h_PtvPt;
  TH2F* h_MvM;

private:

  static vector<string> histnames;
  
  

};