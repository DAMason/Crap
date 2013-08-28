// -*- C++ -*-
//
// Package:    SusyNtuplizer
// Class:      CrapPhoAnalysis.cc
// 
/*

 Description: an example analyzer using SUSY Ntuples

 Implementation:
 The macro is driven by ana.C in this directory.
 Fills MET and diEMPt histograms for gg and ff events with >= 1 good jets. gg and ff
 skimming can be done simultaneously by setting CopyEvents to true in ana.C.

*/
//
// Original Author:  Dongwook Jang
// $Id: CrapPhoAnalysis.cc,v 1.21 2013/05/11 14:14:45 yiiyama Exp $
//

#include <TH1F.h>
#include <TFile.h>
#include <TTree.h>

#include <cmath>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <map>
#include <utility>

// Not particularly enamored with this, but since there seem to be several places scattered around
// where the Jet correction code is enabled, resorting to preprocessor directives to switch this
// code in or out.  Ultimately controlled by this #define:

//#define CMSSWENVIRONMENT=true

// uncomment above to enable changing the jet corrections (requires a CMSSW environment).


#ifdef CMSSWENVIRONMENT // protecting if not in CMSSW environment

#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h" // to access the JEC scales
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h" // to access the uncertainties

#endif //CMSSWENVIRONMENT

#include "../Crap.h"
#include "../UtilPieces.h"

#include "CrapPhoAnalysis.h"
#include "../PhoHistClass.h"


////////// MAIN ANALYSIS FUNCTION //////////



void

CrapPhoAnalysis::Run()
{
  // would be good to define what the indices actually end up meaning...
  // 0 total number of events before lumi mask -- should be total read
  // Then from below:
  //    out << " passed preselection                                          : " << nCnt[1] << " (" << nCnt[1]/float(nCnt[0]) << ")" << std::endl;
  //    out << " goodPhotons >= 2                                             : " << nCnt[2] << " (" << nCnt[2]/float(nCnt[1]) << ")" << std::endl;
  //    out << " fakePhotons >= 2                                             : " << nCnt[3] << " (" << nCnt[3]/float(nCnt[1]) << ")" << std::endl;
  //    out << " alternate fakePhotons >= 2                                   : " << nCnt[12] << " (" << nCnt[12]/float(nCnt[1]) << ")" << std::endl;
  //    out << " elecPhotons >= 2                                             : " << nCnt[13] << " (" << nCnt[13]/float(nCnt[1]) << ")" << std::endl;
  //    out << " elecPhotons in Z window >= 2                                 : " << nCnt[14] << " (" << nCnt[14]/float(nCnt[1]) << ")" << std::endl;
  //    out << " goodPhotons >= 2 && 0 muon                                   : " << nCnt[4] << " (" << nCnt[4]/float(nCnt[1]) << ")" << std::endl;
  //    out << " fakePhotons >= 2 && 0 muon                                   : " << nCnt[5] << " (" << nCnt[5]/float(nCnt[1]) << ")" << std::endl;
  //    out << " goodPhotons >= 2 && 0 lepton                                 : " << nCnt[6] << " (" << nCnt[6]/float(nCnt[1]) << ")" << std::endl;
  //    out << " fakePhotons >= 2 && 0 lepton                                 : " << nCnt[7] << " (" << nCnt[7]/float(nCnt[1]) << ")" << std::endl;
  //    out << " goodPhotons >= 2 && 0 lepton && >= 1 good jet                : " << nCnt[8] << " (" << nCnt[8]/float(nCnt[1]) << ")" << std::endl;
  //    out << " goodPhotons >= 2 && 0 lepton && >= 1 good jet                : " << nCnt[9] << " (" << nCnt[9]/float(nCnt[1]) << ")" << std::endl;
  //    out << " goodPhotons >= 2 && 0 lepton && >= 1 good jet && MET > 50 GeV: " << nCnt[10] << " (" << nCnt[10]/float(nCnt[1]) << ")" << std::endl;
  //    out << " goodPhotons >= 2 && 0 lepton && >= 1 good jet && MET > 50 GeV: " << nCnt[11] << " (" << nCnt[11]/float(nCnt[1]) << ")" << std::endl;

  int nCnt[20];
  for(int i(0); i<20; i++) nCnt[i] = 0;

  ////////// TEXT OUTPUT //////////

  std::ofstream outFile;
  if(logFileName != "cout"){
    outFile.open(logFileName);
    if(!outFile.is_open()){
      std::cerr << "Log output " << logFileName << " could not be opened." << std::endl;
      return;
    }
  }
  std::ostream& out(logFileName == "cout" ? std::cout : outFile);


  
  TFile* ggFile(0);
  TFile* ffFile(0);
  TFile* ffaltFile(0);
  TFile* eeFile(0);
  TFile* fout(0);
  

  try{

    ////////// INITIALIZE SKIMMED CLONE //////////
    TTree* ggTree(0);
    TTree* ffTree(0);
    TTree* ffaltTree(0);
    TTree* eeTree(0);

    if(copyEvents){

      if(printLevel > 0) out << "Open file for skim output" << std::endl;

      ggFile = TFile::Open("susyEvents_" + outputName + "_gg.root", "RECREATE");
      ffFile = TFile::Open("susyEvents_" + outputName + "_ff.root", "RECREATE");
      ffaltFile = TFile::Open("susyEvents_" + outputName + "_ffalt.root", "RECREATE");
      eeFile = TFile::Open("susyEvents_" + outputName + "_ee.root", "RECREATE");


      if(!ggFile || ggFile->IsZombie() || !ffFile || ffFile->IsZombie() || !ffaltFile || ffaltFile->IsZombie() || !eeFile || eeFile->IsZombie() ){
        std::cerr << "Cannot open output file susyEvents_" << outputName << ".root" << std::endl;
        throw std::runtime_error("IOError");
      }
      else{
        ggFile->cd();
        ggTree = new TTree("susyTree", "SUSY Event");
        ggTree->SetAutoSave(10000000);

        ffFile->cd();
        ffTree = new TTree("susyTree", "SUSY Event");
        ffTree->SetAutoSave(10000000);
        
        ffaltFile->cd();
        ffaltTree = new TTree("susyTree", "SUSY Event");
        ffaltTree->SetAutoSave(10000000);
        
        eeFile->cd();
        eeTree = new TTree("susyTree", "SUSY Event");
        eeTree->SetAutoSave(10000000);


        /* Register the output trees to the Event object - the branches will be booked internally */
        event.addOutput(*ggTree);
        event.addOutput(*ffTree);
        event.addOutput(*ffaltTree);
        event.addOutput(*eeTree);

      }

    }
    
    


    ////////// INITIALIZE HISTOGRAM OUTPUT //////////
    if(printLevel > 0) out << "Open file for histograms" << std::endl;

    fout = TFile::Open("hist_" + outputName + ".root", "RECREATE");
    if(!fout || fout->IsZombie()){
      std::cerr << "Cannot open output file hist_" << outputName << ".root" << std::endl;
      throw std::runtime_error("IOError");
    }
    
    PhoHistClass photonhists(fout,"Photons"); // initialize single object histograms...
    PhoHistClass fakehists(fout,"Fakes");
    PhoHistClass altfakehists(fout,"AltFakes");
    PhoHistClass electronhists(fout,"Electrons");
    

    fout->cd();

    if(printLevel > 0) out << "Define histograms" << std::endl;

    TH1F* h_met_gg(new TH1F("h_met_gg","#gamma#gamma #slash{E}_{T};#slash{E}_{T} (GeV);Events / GeV", 200, 0., 200.));
    TH1F* h_met_ff(new TH1F("h_met_ff","ff #slash{E}_{T};#slash{E}_{T} (GeV);Events / GeV", 200, 0., 200.));
    TH1F* h_met_ffAlt(new TH1F("h_met_ffAlt","ffAlt #slash{E}_{T};#slash{E}_{T} (GeV);Events / GeV", 200, 0., 200.));
    TH1F* h_met_ee(new TH1F("h_met_ee","ee #slash{E}_{T};#slash{E}_{T} (GeV);Events / GeV", 200, 0., 200.));
    TH1F* h_diEMPt_gg(new TH1F("h_diEMPt_gg", "#gamma#gamma diEM P_{T};diEM P_{T} (GeV);Events / GeV", 200, 0., 200.));
    TH1F* h_diEMPt_ff(new TH1F("h_diEMPt_ff", "ff diEM P_{T};diEM P_{T} (GeV);Events / GeV", 200, 0., 200.));
    TH1F* h_diEMPt_ffAlt(new TH1F("h_diEMPt_ffAlt", "ffAlt diEM P_{T};diEM P_{T} (GeV);Events / GeV", 200, 0., 200.));
    TH1F* h_diEMPt_ee(new TH1F("h_diEMPt_ee", "ee diEM P_{T};diEM P_{T} (GeV);Events / GeV", 200, 0., 200.));
    TH1F* h_PFDiEMPt_gg(new TH1F("h_PFDiEMPt_gg", "#gamma#gamma PF diEM P_{T};diEM P_{T} (GeV);Events / GeV", 200, 0., 200.));
    TH1F* h_PFDiEMPt_ff(new TH1F("h_PFDiEMPt_ff", "ff PF diEM P_{T};diEM P_{T} (GeV);Events / GeV", 200, 0., 200.));
    TH1F* h_PFDiEMPt_ffAlt(new TH1F("h_PFDiEMPt_ffAlt", "ffAlt PF diEM P_{T};diEM P_{T} (GeV);Events / GeV", 200, 0., 200.));
    TH1F* h_PFDiEMPt_ee(new TH1F("h_PFDiEMPt_ee", "ee PF diEM P_{T};diEM P_{T} (GeV);Events / GeV", 200, 0., 200.));


    /////////////////////////////////////
    ////////// MAIN EVENT LOOP //////////
    /////////////////////////////////////

    if(printLevel > 0) out << "Start event loop" << std::endl;

    long iEntry(0);
    while(iEntry != processNEvents && event.getEntry(iEntry++) != 0){

      if(printLevel > 1 || iEntry % printInterval == 0)
        out << "Begin processing event " << iEntry - 1 << ". Current: run=" << event.runNumber << ", event=" << event.eventNumber << std::endl;

      ////////// DUMP EVENT CONTENT //////////
      if(printLevel > 2) event.Print(out);

      /* total number of events */
      nCnt[0]++;

      ////////// GOOD LUMI FILTER //////////
      if(printLevel > 1) out << "Apply good lumi list." << std::endl;
      if(!IsGoodLumi(event.runNumber, event.luminosityBlockNumber)) continue;

      ////////// MET FILTER //////////
      if(event.isRealData){
        if(printLevel > 1) out << "Apply MET filter." << std::endl;
        if(!event.passMetFilters()) continue;
      }

      ////////// TRIGGER //////////
      if(printLevel > 1) out << "Apply HLT cut." << std::endl;
      if(!PassTriggers()) continue;

      ////////// REQUIRE AT LEAST ONE GOOD VERTEX //////////
      if(printLevel > 1) out << "Require at least one good vertex." << std::endl;
      unsigned nV(event.vertices.size());
      unsigned iV(0);
      for(; iV != nV; ++iV){
        susy::Vertex& vertex(event.vertices[iV]);
        if(vertex.ndof >= 4 && std::abs(vertex.position.Z()) < 24. && vertex.position.Perp() < 2.) break;
      }
      if(iV == nV) continue;

      susy::Vertex const& primVtx(event.vertices[iV]);

      if(printLevel > 1) out << "Event passes preselection." << std::endl;

      /* number of events passing preselection */
      nCnt[1]++;

      ////////// SELECT GOOD AND FAKE PHOTONS //////////
      if(printLevel > 1) out << "Find good barrel photons in the event" << std::endl;

      std::vector<susy::Photon const*> goodPhotons;
      std::vector<susy::Photon const*> fakePhotons;
      std::vector<susy::Photon const*> fakealtPhotons;
      std::vector<susy::Photon const*> electronPhotons;


      susy::PhotonCollection const& photons(event.photons["photons"]);
      unsigned nPhoton(photons.size());
      for(unsigned iP(0); iP != nPhoton; ++iP){
        susy::Photon const& photon(photons[iP]);

        // Fiducial and pT requirements
        double pt(photon.momentum.Pt());
        if(pt < 25.) continue;
        double absEta(std::abs(photon.momentum.Eta()));
        if(absEta > susy::etaGapBegin) continue; /* use barrel photons only */

        // EM requirements
        if (! EMSelector(photon)) continue;

        if (ESelector(photon)) {
          
          if (PhoSelector(photon,event.rho25,"loose")) {
            electronPhotons.push_back(&photon);
            electronhists.Fill(photon);
          }
          
        } else {
          if (PhoSelector(photon,event.rho25,"loose")) {
            goodPhotons.push_back(&photon);
            photonhists.Fill(photon);
          }
          
          if (FakeSelector(photon,event.rho25,"loose")) {
            fakePhotons.push_back(&photon);
            fakehists.Fill(photon);
          }
          
          if (AltFakeSelector(photon,event.rho25,"loose")) {
            fakealtPhotons.push_back(&photon);
            altfakehists.Fill(photon);
          }
          
        }
        
      
      } // end loop over photons...

      ////////// SORT SELECTED PHOTONS //////////
      std::sort(goodPhotons.begin(), goodPhotons.end(), PtGreater<susy::Photon>);
      std::sort(electronPhotons.begin(), electronPhotons.end(), PtGreater<susy::Photon>);
      std::sort(fakePhotons.begin(), fakePhotons.end(), PtGreater<susy::Photon>);
      std::sort(fakealtPhotons.begin(),fakealtPhotons.end(), PtGreater<susy::Photon>);
      
      // there's a continue back to the event loop here...
      if(electronPhotons.size()+goodPhotons.size()+fakePhotons.size() < 2 || electronPhotons.size()+goodPhotons.size()+fakealtPhotons.size() < 2) continue;
      // i.e. dropping out of loop if we don't have some combination of 2 useful EM objects...
      
      
      // Here's what needed to happen to get to this point:
      // if(!IsGoodLumi(event.runNumber, event.luminosityBlockNumber))
      // if(!event.passMetFilters())
      // if(!PassTriggers())
      // REQUIRE AT LEAST ONE GOOD VERTEX
      // 
      // Need either (electron+photon+fake) or (electron+photon+altFake) has to be 2 or more -- both 25 GeV
      
      
      
      if(goodPhotons.size() >= 2 && fakePhotons.size() >= 2){
        out << "Run " << event.runNumber << " Lumi " << event.luminosityBlockNumber << " Event " << event.eventNumber;
        out << " has >= 2 good photons AND >= 2 fake photons. Check for pathologies!" << std::endl;
      }
      
      // Just some counting...

      /* number of events with two good photons */
      if(goodPhotons.size() >= 2 && goodPhotons[0]->momentum.Pt() > 40.0) nCnt[2]++;
      /* number of events with two fake photons */
      if(fakePhotons.size() >= 2 && fakePhotons[0]->momentum.Pt() > 40.0) nCnt[3]++;
      /* number of events with two alternately defined fake photons */
      if(fakealtPhotons.size() >= 2 && fakealtPhotons[0]->momentum.Pt() > 40.0) nCnt[12]++;
      /* number of events with two electronish photons */
      if(electronPhotons.size() >= 2 && electronPhotons[0]->momentum.Pt() > 40.0) {
        nCnt[13]++;
        // hacky way to do this:
        float eemass=(electronPhotons[0]->momentum+electronPhotons[1]->momentum).M();
        if (eemass>80.0 && eemass<110.0) nCnt[14]++;
      }
      // number of events with two good photons and any or no muon
      if(goodPhotons.size() >= 2) nCnt[4]++;
      // number of events with two fake photons and any or no muon
      if(fakePhotons.size() >= 2) nCnt[5]++;


      //////////////////////////////////// Build vector of loose muons
      
      vector<susy::Muon const *> looseMuons;
      

      if(printLevel > 1) out << "Find loose muons in the event" << std::endl;

      susy::MuonCollection const& muons(event.muons["muons"]);
      unsigned nMuon(muons.size());
      unsigned iMuon(0);
      for(; iMuon != nMuon; ++iMuon){
        susy::Muon const& muon(muons[iMuon]);
        if (MuonSelector(muon)) looseMuons.push_back(&muon);
      }

      std::sort(looseMuons.begin(),looseMuons.end(),PtGreater<susy::Muon>);
      
      
      
      //////////////////////////////////// Build vector of loose electrons

      vector<susy::Electron const *> looseElectrons;
      
       
      if(printLevel > 1) out << "Find loose electrons in the event" << std::endl;

      susy::ElectronCollection const& electrons(event.electrons["gsfElectrons"]);
      unsigned nEle(electrons.size());
      unsigned iEle(0);
      for(; iEle != nEle; ++iEle){
        susy::Electron const& electron(electrons[iEle]);
        if (ElectronSelector(electron,primVtx,event.rho25)) looseElectrons.push_back(&electron);
      }
      
      std::sort(looseElectrons.begin(),looseElectrons.end(),PtGreater<susy::Electron>);


      ///////////////////////////////////// Build vector of pf jets
      
      
      std::vector<susy::PFJet const*> keptJets;
      
      if(printLevel > 1) out << "Find good PFJets in the event" << std::endl;

      susy::PFJetCollection const& pfJets(event.pfJets["ak5"]);
      unsigned nJets(pfJets.size());
      unsigned nGoodJets(0);
      for(unsigned iJ(0); iJ != nJets; ++iJ){
        susy::PFJet const& jet(pfJets[iJ]);

        if (PFJetSelector(jet)) {
          
          // need still to clean photons out of this...
          
          keptJets.push_back(&jet);
          ++nGoodJets;

          }

      }
      
      
      std::sort(keptJets.begin(),keptJets.end(),PtGreater<susy::PFJet>);

      
      if(nGoodJets > 0){
        /* number of events with two good photons, no lepton, and >=1 good jets */
        if(goodPhotons.size() >= 2) nCnt[8]++;
        /* number of events with two fake photons, no lepton, and >=1 good jets */
        if(fakePhotons.size() >= 2) nCnt[9]++;
      }
      
      
      
      

      ////////// MET //////////

      
      TVector2 const& metV(event.metMap["pfType01CorrectedMet"].mEt);

      if(nGoodJets > 0 && metV.Mod() > 50.){
        /* number of events with two good photons, no lepton, >=1 good jets, and MET > 50 GeV */
        if(goodPhotons.size() >= 2) nCnt[10]++;
        /* number of events with two fake photons, no lepton, >=1 good jets, and MET > 50 GeV */
        if(fakePhotons.size() >= 2) nCnt[11]++;
      }

      
      
      ////////// PF PARTICLES (DEMONSTRATION OF THE WORKAROUND FOR THE BUG IN TAG cms538v0 / cms538v1) ///////////
      std::map<std::pair<int, float>, susy::PFParticle const*> uniquePF;

      unsigned nPF(event.pfParticles.size());
      for(unsigned iPF(0); iPF != nPF; ++iPF){
        susy::PFParticle const& particle(event.pfParticles[iPF]);
        uniquePF[std::pair<int, float>(particle.pdgId, particle.momentum.Pt())] = &particle;
      }

      std::vector<susy::PFParticle const*> pfParticles;
      for(std::map<std::pair<int, float>, susy::PFParticle const*>::iterator pfItr(uniquePF.begin()); pfItr != uniquePF.end(); ++pfItr)
        pfParticles.push_back(pfItr->second);

      nPF = pfParticles.size();

      
      
      // Now we have required there be two 25 GeV EM objects in some combination of electron+photon+fake or electron+photon+altfake
      // We have filled vectors of the various photon flavors, muons, electrons and jets as well.
      
      
      
      
      
      ////////// FILL HISTOGRAMS //////////

      
      
      
      
      
      // number of events with two good photons and no lepton
      if(goodPhotons.size() >= 2) nCnt[6]++;
      // number of events with two fake photons and no lepton
      if(fakePhotons.size() >= 2) nCnt[7]++;
      
      
      


      ////////// FILL SKIMS //////////
      if(copyEvents){
        if(fakePhotons.size() >=2) ffTree->Fill();
        if(goodPhotons.size() >=2) ggTree->Fill();
        if(fakealtPhotons.size() >= 2) ffaltTree->Fill();
        if(electronPhotons.size() >=2) eeTree->Fill();
      }

    }

    ////////// END OF EVENT LOOP //////////

    out << " ----------------- Job Summary ----------------- " << std::endl;
    out << " Total events                                                 : " << nCnt[0] << std::endl;
    if(nCnt[0] > 0){
      out << " passed preselection                                          : " << nCnt[1] << " (" << nCnt[1]/float(nCnt[0]) << ")" << std::endl;
      out << " goodPhotons >= 2                                             : " << nCnt[2] << " (" << nCnt[2]/float(nCnt[1]) << ")" << std::endl;
      out << " fakePhotons >= 2                                             : " << nCnt[3] << " (" << nCnt[3]/float(nCnt[1]) << ")" << std::endl;
      out << " alternate fakePhotons >= 2                                   : " << nCnt[12] << " (" << nCnt[12]/float(nCnt[1]) << ")" << std::endl;
      out << " elecPhotons >= 2                                             : " << nCnt[13] << " (" << nCnt[13]/float(nCnt[1]) << ")" << std::endl;
      out << " elecPhotons in Z window >= 2                                 : " << nCnt[14] << " (" << nCnt[14]/float(nCnt[1]) << ")" << std::endl;
      out << " goodPhotons >= 2 && 0 muon                                   : " << nCnt[4] << " (" << nCnt[4]/float(nCnt[1]) << ")" << std::endl;
      out << " fakePhotons >= 2 && 0 muon                                   : " << nCnt[5] << " (" << nCnt[5]/float(nCnt[1]) << ")" << std::endl;
      out << " goodPhotons >= 2 && 0 lepton                                 : " << nCnt[6] << " (" << nCnt[6]/float(nCnt[1]) << ")" << std::endl;
      out << " fakePhotons >= 2 && 0 lepton                                 : " << nCnt[7] << " (" << nCnt[7]/float(nCnt[1]) << ")" << std::endl;
      out << " goodPhotons >= 2 && 0 lepton && >= 1 good jet                : " << nCnt[8] << " (" << nCnt[8]/float(nCnt[1]) << ")" << std::endl;
      out << " goodPhotons >= 2 && 0 lepton && >= 1 good jet                : " << nCnt[9] << " (" << nCnt[9]/float(nCnt[1]) << ")" << std::endl;
      out << " goodPhotons >= 2 && 0 lepton && >= 1 good jet && MET > 50 GeV: " << nCnt[10] << " (" << nCnt[10]/float(nCnt[1]) << ")" << std::endl;
      out << " goodPhotons >= 2 && 0 lepton && >= 1 good jet && MET > 50 GeV: " << nCnt[11] << " (" << nCnt[11]/float(nCnt[1]) << ")" << std::endl;
    }

    if(printLevel > 0) out << "Save outputs" << std::endl;

    ////////// END OF EVENT LOOP //////////

    fout->cd();
    fout->Write();

    if(copyEvents){
      event.releaseTree(*ggTree);
      event.releaseTree(*ffTree);
      event.releaseTree(*ffaltTree);
      event.releaseTree(*eeTree);


      ggFile->cd();
      ggFile->Write();

      ffFile->cd();
      ffFile->Write();
      
      ffaltFile->cd();
      ffaltFile->Write();
      
      eeFile->cd();
      eeFile->Write();
      
    }
  }
  catch(std::exception& e){
    std::cerr << e.what() << std::endl;

    event.releaseTrees();
  }



  delete ggFile;
  delete ffFile;
  delete ffaltFile;
  delete eeFile;

  delete fout;
}

