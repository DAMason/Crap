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

#include "../Crap.h"
#include "../UtilPieces.h"

#include "CrapPhoAnalysis.h"
#include "../PhoHistClass.h"
#include "../CounterClass.h"
#include "../DiObjectPlotClass.h"


////////// MAIN ANALYSIS FUNCTION //////////



void

CrapPhoAnalysis::Run()
{  
  
 

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
  TFile* egFile(0);
  TFile* fout(0);
  

  try{

    ////////// INITIALIZE SKIMMED CLONE //////////
    TTree* ggTree(0);
    TTree* ffTree(0);
    TTree* ffaltTree(0);
    TTree* eeTree(0);
    TTree* egTree(0);

    if(copyEvents){

      if(printLevel > 0) out << "Open file for skim output" << std::endl;

      ggFile = TFile::Open("susyEvents_" + outputName + "_gg.root", "RECREATE");
      ffFile = TFile::Open("susyEvents_" + outputName + "_ff.root", "RECREATE");
      ffaltFile = TFile::Open("susyEvents_" + outputName + "_ffalt.root", "RECREATE");
      eeFile = TFile::Open("susyEvents_" + outputName + "_ee.root", "RECREATE");
      egFile = TFile::Open("susyEvents_" + outputName + "_eg.root", "RECREATE");
      

      if(!ggFile || ggFile->IsZombie() || !ffFile || ffFile->IsZombie() || !ffaltFile || ffaltFile->IsZombie() || !eeFile || eeFile->IsZombie() || !egFile || egFile->IsZombie()){
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
        
        egFile->cd();
        egTree = new TTree("susyTree", "SUSY Event");
        egTree->SetAutoSave(10000000);


        /* Register the output trees to the Event object - the branches will be booked internally */
        event.addOutput(*ggTree);
        event.addOutput(*ffTree);
        event.addOutput(*ffaltTree);
        event.addOutput(*eeTree);
        event.addOutput(*egTree);


      } // else from checking for bad root files

    } // if copyEvents
    
    


    ////////// INITIALIZE HISTOGRAM OUTPUT //////////
    if(printLevel > 0) out << "Open file for histograms" << std::endl;

    fout = TFile::Open("hist_" + outputName + ".root", "RECREATE");
    if(!fout || fout->IsZombie()){
      std::cerr << "Cannot open output file hist_" << outputName << ".root" << std::endl;
      throw std::runtime_error("IOError");
    }
    
    
    
    // Initialize some marginally intelligible counters...
    
    CounterClass NRead("NRead");
    CounterClass NPresel("NPresel");
    
    CounterClass Ngg("Ngg");
    CounterClass Nff("Nff");
    CounterClass NffAlt("NffAlt");
    CounterClass Nee("Nee");
    CounterClass NeeZ("NeeZ");
    CounterClass Nfg("Nfg");
    CounterClass Ngf("Ngf");
    CounterClass Neg("Neg");
    CounterClass NfAltg("NfAltg");
    CounterClass NgfAlt("NgfAlt");
    
    
    
    PhoHistClass photonhists(fout,"Photons"); // initialize single object histograms...
    PhoHistClass fakehists(fout,"Fakes");
    PhoHistClass altfakehists(fout,"AltFakes");
    PhoHistClass electronhists(fout,"Electrons");
    PhoHistClass diphotonleadhists(fout,"DiphotonLead");
    PhoHistClass diphotontrailhists(fout,"DiphotonTrail");
    
    
    DiObjectPlotClass diphotonhists(fout,"Diphotons"); // initialize di-object histograms...
    DiObjectPlotClass difakehists(fout,"DiFake");
    DiObjectPlotClass dielectronhists(fout,"DiElectron");
    DiObjectPlotClass egammahists(fout,"Egamma");
    DiObjectPlotClass dialtfakehists(fout,"DiAltFake");
    DiObjectPlotClass fakegammahists(fout,"FakeGamma");
    DiObjectPlotClass gammafakehists(fout,"GammaFake");
    DiObjectPlotClass altfakegammahists(fout,"AltFakeGamma");
    DiObjectPlotClass gammaaltfakehists(fout,"GammaAltFake");
    
    
    

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
      NRead++;

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
      NPresel++;

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
      if(electronPhotons.size()+goodPhotons.size()+fakePhotons.size() < 2 && electronPhotons.size()+goodPhotons.size()+fakealtPhotons.size() < 2) continue;
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
      
      
      // Got various flavors of photons -- now get other bits...
      

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

      

      
      
      
      

      ////////// MET //////////

      
      TVector2 const& metV(event.metMap["pfType01CorrectedMet"].mEt);


      
      
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
      
      ////////// Event Classification //////////
      
      bool IsGGEvent=false;
      bool IsFFEvent=false;
      bool IsEEEvent=false;
      bool IsEGEvent=false;
      bool IsFGEvent=false;
      bool IsGFEvent=false;
      
      bool IsAltFFEvent=false;
      bool IsAltGFEvent=false;
      bool IsAltFGEvent=false;
      
      
      if (goodPhotons.size()>=2 && goodPhotons[0]->momentum.Pt()>40.0) {
        Ngg++;
        diphotonleadhists.Fill(*goodPhotons[0]);
        diphotontrailhists.Fill(*goodPhotons[1]);
        diphotonhists.Fill(goodPhotons[0]->momentum,goodPhotons[1]->momentum);
        IsGGEvent=true;
      }
      
      if (electronPhotons.size()>=2 && electronPhotons[0]->momentum.Pt()>40.0) {
        Nee++;
        dielectronhists.Fill(electronPhotons[0]->momentum,electronPhotons[1]->momentum);
        IsEEEvent=true;
      }
      
      if (fakealtPhotons.size()>=2 && fakealtPhotons[0]->momentum.Pt()>40.0) {
        NffAlt++;
        dialtfakehists.Fill(fakealtPhotons[0]->momentum,fakealtPhotons[1]->momentum);
        IsAltFFEvent=true;
      }
      
      if (fakePhotons.size()>=2 && fakePhotons[0]->momentum.Pt()>40.0) {
        Nff++;
        difakehists.Fill(fakePhotons[0]->momentum,fakePhotons[1]->momentum);
        IsFFEvent=true;
      }
      
      if (goodPhotons.size()>=1 && electronPhotons.size()>=1
                                && !IsGGEvent
                                && !IsEEEvent
                                && (goodPhotons[0]->momentum.Pt() > 40.0 || electronPhotons[0]->momentum.Pt()>40.0)) {
          Neg++;
          egammahists.Fill(goodPhotons[0]->momentum,electronPhotons[0]->momentum);
          IsEGEvent=true;
      }
      
      if (goodPhotons.size()>=1 && fakePhotons.size()>=1
                                && !IsGGEvent
                                && !IsFFEvent) {
        if (goodPhotons[0]->momentum.Pt() > 40.0 && (goodPhotons[0]->momentum.Pt() > fakePhotons[0]->momentum.Pt()) ) {
          Ngf++;
          gammafakehists.Fill(goodPhotons[0]->momentum,fakePhotons[0]->momentum);
          IsGFEvent=true;
        }
        if (fakePhotons[0]->momentum.Pt() > 40.0 && (fakePhotons[0]->momentum.Pt() > goodPhotons[0]->momentum.Pt()) ) {
          Nfg++;
          fakegammahists.Fill(fakePhotons[0]->momentum,goodPhotons[0]->momentum);
          IsFGEvent=true;
        } 
      }// f+g if
      
      if (goodPhotons.size()>=1 && fakealtPhotons.size()>=1
                                && !IsGGEvent
                                && !IsAltFFEvent) {
        if (goodPhotons[0]->momentum.Pt() > 40.0 && (goodPhotons[0]->momentum.Pt() > fakealtPhotons[0]->momentum.Pt()) ) {
          NgfAlt++;
          altfakegammahists.Fill(fakealtPhotons[0]->momentum,goodPhotons[0]->momentum);
          IsAltGFEvent=true;
        }
        if (fakealtPhotons[0]->momentum.Pt() > 40.0 && (fakealtPhotons[0]->momentum.Pt() > goodPhotons[0]->momentum.Pt()) ) {
          NfAltg++;
          gammaaltfakehists.Fill(goodPhotons[0]->momentum,fakealtPhotons[0]->momentum);
          IsAltFGEvent=true;
        }
      } // alt f+g if
     
      
      ////////// FILL HISTOGRAMS //////////

      
      
      
      
            
      
      


      ////////// FILL SKIMS //////////
      if(copyEvents){
        if(IsFFEvent) ffTree->Fill();
        if(IsGGEvent) ggTree->Fill();
        if(IsAltFFEvent) ffaltTree->Fill();
        if(IsEEEvent) eeTree->Fill();
        if(IsEGEvent) egTree->Fill();
      }

    }

    ////////// END OF EVENT LOOP //////////

    out << " ----------------- Job Summary ----------------- " << std::endl;
    out << " Total events                                                 : " << NRead.Val() << std::endl;
    if(NRead.Val() > 0){
      out << " passed preselection                                          : " << NPresel.Val() << " (" << NPresel.Val()/float(NRead.Val()) << ")" << std::endl;
      out << " goodPhotons >= 2                                             : " << Ngg.Val()     << " (" << Ngg.Val()/float(NPresel.Val()) << ")" << std::endl;
      out << " fakePhotons >= 2                                             : " << Nff.Val()     << " (" << Nff.Val()/float(NPresel.Val()) << ")" << std::endl;
      out << " fake-gamma                                                   : " << Nfg.Val()     << " (" << Nfg.Val()/float(NPresel.Val()) << ")" << std::endl;
      out << " gamma-fake                                                   : " << Ngf.Val()     << " (" << Ngf.Val()/float(NPresel.Val()) << ")" << std::endl;
      out << " alternate fakePhotons >= 2                                   : " << NffAlt.Val()  << " (" << NffAlt.Val()/float(NPresel.Val()) << ")" << std::endl;
      out << " alternate fake-gamma                                         : " << NfAltg.Val()  << " (" << NfAltg.Val()/float(NPresel.Val()) << ")" << std::endl;
      out << " gamma-alternate fake                                         : " << NgfAlt.Val()  << " (" << NgfAlt.Val()/float(NPresel.Val()) << ")" << std::endl;
      out << " elecPhotons >= 2                                             : " << Nee.Val()     << " (" << Nee.Val()/float(NPresel.Val()) << ")" << std::endl;
      out << " elecPhotons in Z window >= 2                                 : " << NeeZ.Val()    << " (" << NeeZ.Val()/float(NPresel.Val()) << ")" << std::endl;
      out << " e-gamma (or gamma-e)                                         : " << Neg.Val()     << " (" << Neg.Val()/float(NPresel.Val()) << ")" << std::endl;
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
      event.releaseTree(*egTree);

      

      ggFile->cd();
      ggFile->Write();

      ffFile->cd();
      ffFile->Write();
      
      ffaltFile->cd();
      ffaltFile->Write();
      
      eeFile->cd();
      eeFile->Write();
      
      egFile->cd();
      egFile->Write();
      
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
  delete egFile;


  delete fout;
}

