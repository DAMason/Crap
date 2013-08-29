//
//  PhoHistClass.h
//  CrapProject
//
//  Created by David Mason on 8/26/13.
//  Copyright (c) 2013 David Mason. All rights reserved.
//
//  The idea of this is to encapsulate all the histograms of single photon objects -- so that one can imagine filling/booking things in one shot

#ifndef __CrapProject__PhoHistClass__
#define __CrapProject__PhoHistClass__

#include <iostream>
#include "SusyEvent.h"
#include <TH1F.h>
#include <TFile.h>

#endif /* defined(__CrapProject__PhoHistClass__) */


using namespace std;


class PhoHistClass {
public:
  PhoHistClass(TFile*,string);  // Arguments are file things are going into and a string that defines the group of histo's (appended into name, etc.)
  virtual ~PhoHistClass();
  void BookHists();
  void Fill(susy::Photon const& pho,float weight);
  void Fill(susy::Photon const& pho); // default weight of 1.0


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

private:  
  
  static vector<string> histnames;
};

