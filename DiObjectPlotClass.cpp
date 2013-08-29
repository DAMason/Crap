//
//  DiObjectPlotClass.cpp
//  CrapProject
//
//  Created by David Mason on 8/29/13.
//  Copyright (c) 2013 David Mason. All rights reserved.
//

#include "DiObjectPlotClass.h"

vector<string> DiObjectPlotClass::histnames; // need to define/initialize

DiObjectPlotClass::DiObjectPlotClass(TFile* rootfile,string namestring) {
  
  //
  
  // First thing we do is get another string that we strip off whitespace from...
  
  string manglednamestring=namestring;
  manglednamestring.erase(remove(manglednamestring.begin(),manglednamestring.end(), ' '), manglednamestring.end()); // removes all ' '
  
  cout << histnames.size() << " Di- object histo's defined -- this one named: " << manglednamestring << endl;
  
  if (histnames.size()==0 || find(histnames.begin(),histnames.end(),manglednamestring) == histnames.end()) { // if we don't already have it
    histnames.push_back(manglednamestring);
  } else {
    cout << "WARNING!!! Duplicate histo label -- We already have a set of histograms " << endl;
    cout << "           labeled with string: " << namestring << " (becoming " << manglednamestring << ")" << endl;
    cout << "           Likely we're double filling something weirdly later..." << endl;
    return;
  }
  
  rootfile->cd();
  rootfile->mkdir(manglednamestring.c_str());
  rootfile->cd(manglednamestring.c_str());
  
  h_diPt = new TH1F("diPt","diPt",200,0.0,1000.0);
  h_InvMass = new TH1F("InvMass","InvMass",200,0.0,1000.0);
  h_dPhi = new TH1F("dPhi","dPhi",200,-6.29,6.29);
  h_dR = new TH1F("dR","dR",200,0,7);
  
  h_PtvPt = new TH2F("PtvPt","PtvPt",200,0,1000.0,200,0,1000.0);
  h_MvM = new TH2F("MvM","MvM",200,0,1000.0,200,0.0,1000.0);
  
}

DiObjectPlotClass::~DiObjectPlotClass() {
  
}

void DiObjectPlotClass::Fill(TLorentzVector x, TLorentzVector y) {

  Fill(x,y,1.0);

}

void DiObjectPlotClass::Fill(TLorentzVector x, TLorentzVector y,float weight) {
  

  h_diPt->Fill((x+y).Pt(),weight);
  
  h_InvMass->Fill((x+y).M(),weight);
  
  h_dPhi->Fill(x.DeltaPhi(y),weight);
  
  h_dR->Fill(x.DeltaR(y),weight);
  
  h_PtvPt->Fill(x.Pt(),y.Pt(),weight);
  
  h_MvM->Fill(x.M(),y.M(),weight);
  
  
}
