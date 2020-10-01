
//C++ headers
#include<vector>

//ROOT headers
#include "TF1.h"
#include "TRandom3.h"
#include "TParticle.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TLorentzVector.h"

//local headers
#include "TDecayPolarized.h"

using namespace std;

//_____________________________________________________________________________
TDecayPolarized::TDecayPolarized(Int_t pdg, Double_t alpha) {

  if(TMath::Abs(pdg) != 11 and TMath::Abs(pdg) != 13) {
    cout << "Error in TDecayPolarized, only dielectron or dimuon decays implemented (" << pdg << ")" << endl;
    exit(1);
  }

  fPol = new TF1("dsigdcostheta","1.+[0]*x*x",-1.,1.);
  //amount of polarization, +1: full transverse, -1: full longitudinal, 0: unpolarized
  fPol->SetParameter(0, alpha);

  fRand = new TRandom3();
  fRand->SetSeed(5572323);

  TParticlePDG *part = TDatabasePDG::Instance()->GetParticle(pdg);
  fMass = part->Mass();
  fPdg = pdg;

  fVec.resize(2);

}//TDecayPolarized

//_____________________________________________________________________________
TDecayPolarized::~TDecayPolarized() {

  delete fPol;
  delete fRand;

}//~TDecayPolarized

//_____________________________________________________________________________
void TDecayPolarized::Generate(const TLorentzVector &vm) {

  //energies and momenta in lab frame 
  Double_t e1 = vm.M() / 2.;
  Double_t p1 = TMath::Sqrt((e1 + fMass)*(e1 - fMass));

  Double_t costheta = fPol->GetRandom();

  Double_t sintheta = TMath::Sqrt((1. + costheta)*(1. - costheta));
  Double_t phi = 2. * TMath::Pi() * fRand->Rndm();
  Double_t px1 = p1 * sintheta * TMath::Cos(phi);
  Double_t py1 = p1 * sintheta * TMath::Sin(phi);
  Double_t pz1 = p1 * costheta;

  TLorentzVector v1, v2;
  v1.SetPxPyPzE(px1, py1, pz1, e1); //in vector meson rest frame
  v2.SetPxPyPzE(-px1, -py1, -pz1, e1);

  //helicity frame, polarization axis is direction of vm in the CM 
  TVector3 v3vm = (vm.Vect()).Unit();
  v1.RotateUz(v3vm);
  v2.RotateUz(v3vm);

  //boost decay particles from vector meson rest frame to CM
  TVector3 betavmcm(-1./vm.E()*vm.Vect());
  v1.Boost(-betavmcm);
  v2.Boost(-betavmcm);

  fVec[0].SetMomentum(v1);
  fVec[1].SetMomentum(v2);

  if( fRand->Rndm() > 0.5 ) {
    fVec[0].SetPdgCode(fPdg);
    fVec[1].SetPdgCode(-fPdg);
  } else {
    fVec[1].SetPdgCode(fPdg);
    fVec[0].SetPdgCode(-fPdg);
  }

}//Generate

//_____________________________________________________________________________
const TParticle* TDecayPolarized::GetDecay(Int_t idx) const {

  if(idx < 0 or idx > 1) return 0x0;

  return &fVec[idx];

}//GetDecay







































