template<typename T>
bool
PtGreater(const T* p1, const T* p2) {
  return p1->momentum.Pt() > p2->momentum.Pt();
}

template<typename T1, typename T2>
bool
isSameObject(const T1& p1, const T2& p2, double dRCut = 0.1)
{
  float dEta = p1.momentum.Eta() - p2.momentum.Eta();
  float dPhi = TVector2::Phi_mpi_pi(p1.momentum.Phi() - p2.momentum.Phi());
  return dEta*dEta + dPhi*dPhi < dRCut * dRCut;
}

void
photonEffectiveAreas(double _eta, double* _effA)
{
  double& effACH(_effA[0]);
  double& effANH(_effA[1]);
  double& effAPh(_effA[2]);

  // Source: CutBasedPhotonID2012 twiki
  if(_eta < 1.){
    effACH = 0.012;
    effANH = 0.03;
    effAPh = 0.148;
  }
  else if(_eta < 1.479){
    effACH = 0.010;
    effANH = 0.057;
    effAPh = 0.13;
  }
  else if(_eta < 2.){
    effACH = 0.014;
    effANH = 0.039;
    effAPh = 0.112;
  }
  else if(_eta < 2.2){
    effACH = 0.012;
    effANH = 0.015;
    effAPh = 0.216;
  }
  else if(_eta < 2.3){
    effACH = 0.016;
    effANH = 0.024;
    effAPh = 0.262;
  }
  else if(_eta < 2.4){
    effACH = 0.02;
    effANH = 0.039;
    effAPh = 0.26;
  }
  else{
    effACH = 0.012;
    effANH = 0.072;
    effAPh = 0.266;
  }
}



// More intuitive version of these:

float rhoCorrectedChIso(float iso,float eta,float rho) {
    float corrIso=iso;
    float abseta=fabs(eta);
    if (abseta<=1.0) {
      corrIso=iso - rho*0.012;
    } else if ( abseta > 1.0 && abseta<=1.479) {
      corrIso=iso - rho*0.010;
    } else if ( abseta > 1.479 && abseta<=2.0) {
      corrIso=iso - rho*0.014;
    } else if ( abseta > 2.0 && abseta<=2.2) {
      corrIso=iso - rho*0.012;
    } else if ( abseta > 2.2 && abseta<=2.3) {
      corrIso=iso - rho*0.016;
    } else if ( abseta > 2.3 && abseta<=2.4) {
      corrIso=iso - rho*0.020;
    } else { // abseta>2.4
      corrIso=iso - rho*0.012;
    }  
  
  return(fmax(corrIso,0.)); 
}


float rhoCorrectedNIso(float iso,float eta,float rho) {
    float corrIso=iso;
    float abseta=fabs(eta);
    if (abseta<=1.0) {
      corrIso=iso - rho*0.030;
    } else if ( abseta > 1.0 && abseta<=1.479) {
      corrIso=iso - rho*0.057;
    } else if ( abseta > 1.479 && abseta<=2.0) {
      corrIso=iso - rho*0.039;
    } else if ( abseta > 2.0 && abseta<=2.2) {
      corrIso=iso - rho*0.015;
    } else if ( abseta > 2.2 && abseta<=2.3) {
      corrIso=iso - rho*0.024;
    } else if ( abseta > 2.3 && abseta<=2.4) {
      corrIso=iso - rho*0.039;
    } else { // abseta>2.4
      corrIso=iso - rho*0.072;
    }  
  
  return(fmax(corrIso,0.)); 
}


float rhoCorrectedPhoIso(float iso,float eta,float rho) {
    float corrIso=iso;
    float abseta=fabs(eta);
    if (abseta<=1.0) {
      corrIso=iso - rho*0.148;
    } else if ( abseta > 1.0 && abseta<=1.479) {
      corrIso=iso - rho*0.130;
    } else if ( abseta > 1.479 && abseta<=2.0) {
      corrIso=iso - rho*0.112;
    } else if ( abseta > 2.0 && abseta<=2.2) {
      corrIso=iso - rho*0.216;
    } else if ( abseta > 2.2 && abseta<=2.3) {
      corrIso=iso - rho*0.262;
    } else if ( abseta > 2.3 && abseta<=2.4) {
      corrIso=iso - rho*0.260;
    } else { // abseta>2.4
      corrIso=iso - rho*0.266;
    }  
  
  return(fmax(corrIso,0.)); 
}




void
electronEffectiveAreas(double _eta, double &_effA)
{
  // Source: EgammaEARhoCorrection twiki
  if(_eta < 1.)
    _effA = 0.13;
  else if(_eta < 1.479)
    _effA = 0.14;
  else if(_eta < 2.)
    _effA = 0.07;
  else if(_eta < 2.2)
    _effA = 0.09;
  else if(_eta < 2.3)
    _effA = 0.11;
  else if(_eta < 2.4)
    _effA = 0.11;
  else
    _effA = 0.14;
}


bool 
EMSelector(susy::Photon const& pho) {  // Cuts applied to any and all EM objects...

  // sane R9
  if (pho.r9 <0.01 ) return false;
  if (pho.r9 > 1.0) return false;
  
  // barrel only
  if (!pho.isEB()) return false;
  
  // From standard loose photon ID 
  
  if (pho.hadTowOverEm>=0.05) return false; 
  
  if (pho.sigmaIetaIeta>=0.012) return false;  // assuming we don't try to invert this for making fakes...


return true;
}

bool 
ESelector(susy::Photon const& pho) { // Electron (true) or Photon (false)

  if (pho.nPixelSeeds == 0) return false;

return true;
}


bool
PhoSelector(susy::Photon const& pho, float rho, std::string level) { // Photon (true) or some kind of fake (false) -- further cuts needed on fakes themselves...


  if (level=="medium") {
    if (pho.sigmaIetaIeta>0.011) return false;   // Note additional sigietaieta
    if (rhoCorrectedChIso(pho.chargedHadronIso,pho.momentum.Eta(),rho) >=1.5 ) return false;
    if (rhoCorrectedNIso(pho.neutralHadronIso,pho.momentum.Eta(),rho) >= 1.0 + 0.04*pho.momentum.Et() ) return false;
    if (rhoCorrectedPhoIso(pho.photonIso,pho.momentum.Eta(),rho) >= 0.7 + 0.005*pho.momentum.Et() ) return false;
  } else if (level=="tight") {
    if (pho.sigmaIetaIeta>0.011) return false;   // Note additional sigietaieta
    if (rhoCorrectedChIso(pho.chargedHadronIso,pho.momentum.Eta(),rho) >=0.7 ) return false;
    if (rhoCorrectedNIso(pho.neutralHadronIso,pho.momentum.Eta(),rho) >= 0.4 + 0.04*pho.momentum.Et() ) return false;
    if (rhoCorrectedPhoIso(pho.photonIso,pho.momentum.Eta(),rho) >= 0.5 + 0.005*pho.momentum.Et() ) return false;
  } else {  // Default will be loose
    if (rhoCorrectedChIso(pho.chargedHadronIso,pho.momentum.Eta(),rho) >=2.6) return false;
    if (rhoCorrectedNIso(pho.neutralHadronIso,pho.momentum.Eta(),rho) >= 3.5 + 0.04*pho.momentum.Et() ) return false;
    if (rhoCorrectedPhoIso(pho.photonIso,pho.momentum.Eta(),rho) >= 1.3 + 0.005*pho.momentum.Et() ) return false;
  } // end level if chain

return true;

}


bool
FakeSelector(susy::Photon const& pho, float rho, std::string level) {

  float chIso=rhoCorrectedChIso(pho.chargedHadronIso,pho.momentum.Eta(),rho);
  
  if (level=="medium") {
    if (pho.sigmaIetaIeta>0.011) return false;   // Note additional sigietaieta
    if (chIso<1.5 || chIso>15.0) return false;  // Invert only this guy up to a point -- still need the others:
    if (rhoCorrectedNIso(pho.neutralHadronIso,pho.momentum.Eta(),rho) >= 1.0 + 0.04*pho.momentum.Et() ) return false;
    if (rhoCorrectedPhoIso(pho.photonIso,pho.momentum.Eta(),rho) >= 0.7 + 0.005*pho.momentum.Et() ) return false;
  } else if (level=="tight") {
    if (pho.sigmaIetaIeta>0.011) return false;   // Note additional sigietaieta
    if (chIso<0.7 || chIso>15.0) return false;  // Invert only this guy up to a point -- still need the others:
    if (rhoCorrectedNIso(pho.neutralHadronIso,pho.momentum.Eta(),rho) >= 0.4 + 0.04*pho.momentum.Et() ) return false;
    if (rhoCorrectedPhoIso(pho.photonIso,pho.momentum.Eta(),rho) >= 0.5 + 0.005*pho.momentum.Et() ) return false;
    
  } else {  // Default would be loose
    if (chIso<2.6 || chIso>15.0) return false;  // Invert only this guy up to a point -- still need the others:
    if (rhoCorrectedNIso(pho.neutralHadronIso,pho.momentum.Eta(),rho) >= 3.5 + 0.04*pho.momentum.Et() ) return false;
    if (rhoCorrectedPhoIso(pho.photonIso,pho.momentum.Eta(),rho) >= 1.3 + 0.005*pho.momentum.Et() ) return false;
  }
  
  
return true;

}


bool 
AltFakeSelector(susy::Photon const& pho, float rho, std::string level) {

  float phoIso=rhoCorrectedPhoIso(pho.photonIso,pho.momentum.Eta(),rho);

  if (level=="medium") {
    if (pho.sigmaIetaIeta>0.011) return false;   // Note additional sigietaieta
    if (rhoCorrectedChIso(pho.chargedHadronIso,pho.momentum.Eta(),rho) >=1.5 ) return false;
    if (rhoCorrectedNIso(pho.neutralHadronIso,pho.momentum.Eta(),rho) >= 1.0 + 0.04*pho.momentum.Et() ) return false;
    if ((phoIso< 0.7 + 0.005*pho.momentum.Et()) ||  (phoIso>= 10.0 + 0.005*pho.momentum.Et())) return false;  // Invert this one, guess at high end
  } else if (level=="tight") {
    if (pho.sigmaIetaIeta>0.011) return false;   // Note additional sigietaieta
    if (rhoCorrectedChIso(pho.chargedHadronIso,pho.momentum.Eta(),rho) >=0.7 ) return false;
    if (rhoCorrectedNIso(pho.neutralHadronIso,pho.momentum.Eta(),rho) >= 0.4 + 0.04*pho.momentum.Et() ) return false;
    if ((phoIso< 0.5 + 0.005*pho.momentum.Et()) ||  (phoIso>= 10.0 + 0.005*pho.momentum.Et())) return false;  // Invert this one, guess at high end
  } else {  // Default will be loose
    if (rhoCorrectedChIso(pho.chargedHadronIso,pho.momentum.Eta(),rho) >=2.6) return false;
    if (rhoCorrectedNIso(pho.neutralHadronIso,pho.momentum.Eta(),rho) >= 3.5 + 0.04*pho.momentum.Et() ) return false;
    if ((phoIso< 1.3 + 0.005*pho.momentum.Et()) ||  (phoIso>= 10.0 + 0.005*pho.momentum.Et())) return false;  // Invert this one, guess at high end
  } // end level if chain

return true;

}


