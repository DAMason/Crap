//
//  PhoHistClass.cpp
//  CrapProject
//
//  Created by David Mason on 8/26/13.
//  Copyright (c) 2013 David Mason. All rights reserved.
//

#include "PhoHistClass.h"

/*
// Member data is for the most part just a bunch of histograms
TH1F* h_Pt;
TH1F* h_Eta;
TH1F* h_Phi;
TH1F* h_seedTime;
TH1F* h_nSeeds;
TH1F* h_hOverE;
TH1F* h_sigiEtaiEta;
TH1F* h_sigiPhiiPhi;
TH1F* h_chargedHadIso;
TH1F* h_neutralHadIso;
TH1F* h_photonIso;
TH1F* h_WorstOtherVxchHadIso;
TH1F* h_ecalIso;
TH1F* h_trkIso;
TH1F* h_hcalIso;
TH1F* h_nTrkHollow03;
*/


vector<string> PhoHistClass::histnames; // need to define/initialize

PhoHistClass::PhoHistClass(TFile* rootfile,string namestring) {
  
  //
  
  // First thing we do is get another string that we strip off whitespace from...
  
  string manglednamestring=namestring;
  manglednamestring.erase(remove(manglednamestring.begin(),manglednamestring.end(), ' '), manglednamestring.end()); // removes all ' '
  
  cout << histnames.size() << " Single object histo's defined -- this one named: " << manglednamestring << endl;
  
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
  

  //cout << manglednamestring << endl;
  
  h_Pt = new TH1F("Pt","Pt",200,0.0,1000.0);
  h_Eta = new TH1F("Eta","Eta",200,-4.0,4.0);
  h_Phi = new TH1F("Phi","Phi",200,-6.3,6.3);
  h_seedTime = new TH1F("seedTime","seedTime",200,-25,25);
  h_nSeeds = new TH1F("nSeeds","nSeeds",100,0,100);
  h_hOverE = new TH1F("hOverE","hOverE",120,0,1.2);
  h_sigiEtaiEta = new TH1F("sigiEtaiEta","sigiEtaiEta",100,0,0.1);
  h_sigiPhiiPhi = new TH1F("sigiPhiiPhi","sigiPhiiPhi",100,0,0.1);
  h_chargedHadIso = new TH1F("chargedHadIso","chargedHadIso",300,-10,50);
  h_neutralHadIso = new TH1F("neutralHadIso","neutralHadIso",300,-10,50);
  h_photonIso = new TH1F("photonIso","photonIso",300,-10,50);
  h_WorstOtherVxchHadIso = new TH1F("WorstOtherVtxchHadIso","WorstOtherVtxchHadIso",400,-10,70);
  h_ecalIso = new TH1F("ecalIso","ecalIso",300,-10,50);
  h_trkIso = new TH1F("trkIso","trkIso",300,-10,50);
  h_hcalIso = new TH1F("hcalIso","hcalIso",300,-10,50);
  h_nTrkHollow03 = new TH1F("nTrkHollow03","nTrkHollow03",100,0,100);

}


PhoHistClass::~PhoHistClass()
{
  
}


void PhoHistClass::Fill(susy::Photon const& pho) {  // call weighted version but with 1.0
  Fill(pho,1.0);
}

void PhoHistClass::Fill(susy::Photon const& pho,float weight)
{
  h_Pt->Fill(pho.momentum.Pt(),weight);
  h_Eta->Fill(pho.momentum.Eta(),weight);
  h_Phi->Fill(pho.momentum.Phi(),weight);
  h_seedTime->Fill(pho.seedTime,weight);
  h_nSeeds->Fill(pho.nPixelSeeds,weight);
  h_hOverE->Fill(pho.hadronicOverEm,weight);
  h_sigiEtaiEta->Fill(pho.sigmaIetaIeta,weight);
  h_sigiPhiiPhi->Fill(pho.sigmaIphiIphi,weight);
  h_chargedHadIso->Fill(pho.chargedHadronIso,weight);
  h_neutralHadIso->Fill(pho.neutralHadronIso,weight);
  h_photonIso->Fill(pho.photonIso,weight);
  h_WorstOtherVxchHadIso->Fill(pho.worstOtherVtxChargedHadronIso,weight);
  h_ecalIso->Fill(pho.ecalRecHitSumEtConeDR03,weight);
  h_trkIso->Fill(pho.trkSumPtHollowConeDR03,weight);
  h_hcalIso->Fill(pho.hcalIsoConeDR03_2012,weight);
  h_nTrkHollow03->Fill(pho.nTrkHollowConeDR03,weight);
  
}


