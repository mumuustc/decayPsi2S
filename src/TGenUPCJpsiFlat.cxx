
//C++ headers
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

//ROOT headers
#include "TDatabasePDG.h"
#include "TRandom3.h"
#include "TMath.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"
#include "TParticle.h"

//local headers
#include "TDecayPolarized.h"
#include "TGenUPCJpsiFlat.h"

//_____________________________________________________________________________
TGenUPCJpsiFlat::TGenUPCJpsiFlat(Double_t pt2max, Double_t etamin, Double_t etamax):
  fPt2Max(pt2max), fEtaMin(etamin), fEtaMax(etamax), fNtx(1) {

  fEtaRange = etamax - etamin;

  fPhiMin = -TMath::Pi();
  fPhiRange = 2*TMath::Pi();

  fPdgDat = TDatabasePDG::Instance();
  fMass = fPdgDat->GetParticle(443)->Mass();

  fRand = new TRandom3();
  fRand->SetSeed(5572323);

  fDec = new TDecayPolarized();
  fPart.resize(2);

  fTxOut.open("output.tx");
  evtline[0] = "EVENT: ";
  evtline[1] = " 2 1";
  vtxline = "VERTEX: 0 0 0 0 1 0 0 2";

}//TGenUPCJpsiFlat

//_____________________________________________________________________________
TGenUPCJpsiFlat::~TGenUPCJpsiFlat() {

  fTxOut.close();

  delete fRand;
  delete fDec;

}//~TGenUPCJpsiFlat

//_____________________________________________________________________________
void TGenUPCJpsiFlat::GenerateEvent() {

  //generating loop
  while(1) {

    //initial pT^2, rapidity and phi
    Double_t pt2 = fPt2Max * fRand->Rndm();
    Double_t pt = TMath::Sqrt(pt2);
    Double_t yval = fEtaMin + fEtaRange * fRand->Rndm();
    Double_t phi = fPhiMin + fPhiRange * fRand->Rndm();

    //J/psi Lorentz vector
    TLorentzVector vjpsi;
    vjpsi.SetPtEtaPhiM(pt, eta_from_y(yval, pt), phi, fMass);

    //polarized decay
    fDec->Generate(vjpsi);

    //test for the fiducial eta range
    Double_t eta0 = fDec->GetDecay(0)->Eta();
    Double_t eta1 = fDec->GetDecay(1)->Eta();

    if( (eta0>fEtaMin && eta0<fEtaMax) && (eta1>fEtaMin && eta1<fEtaMax) ) {

      //event accepted by pseudorapidity interval

      fPart[0] = fDec->GetDecay(0);
      fPart[1] = fDec->GetDecay(1);

      break;
    }

  }//generating loop

}//GenerateEvent

//_____________________________________________________________________________
Int_t TGenUPCJpsiFlat::ImportParticles(TClonesArray *particles, Option_t *opt) {

  //load decayed particles to clones array
  particles->Clear();

  new( (*particles)[0] ) TParticle(*fPart[0]);
  new( (*particles)[1] ) TParticle(*fPart[1]);

  return particles->GetEntriesFast();

}//ImportParticles

//_____________________________________________________________________________
void TGenUPCJpsiFlat::WriteStarlight() {

  //output in Starlight format
  std::ostringstream tx;
  tx << evtline[0] << fNtx << evtline[1] << std::endl;
  tx << vtxline << std::endl;
  put_tx_track(tx, 0);
  put_tx_track(tx, 1);
  fTxOut << tx.str();
  fNtx++;

}//

//_____________________________________________________________________________
Double_t TGenUPCJpsiFlat::eta_from_y(Double_t y, Double_t pT) {

  //get pseudorapidity from rapidity, needs also pT and mass
  //
  //equation 125 in Sahoo, Relativistic Kinematics, arXiv:1604.02651

  Double_t mT2 = fMass*fMass + pT*pT;
  Double_t ch2 = TMath::CosH(y);
  ch2 *= ch2;

  Double_t c1 = TMath::Sqrt(mT2*ch2 - fMass*fMass);
  Double_t c2 = TMath::Sqrt(mT2)*TMath::SinH(y);

  return 0.5*TMath::Log( (c1+c2)/(c1-c2) );

}//eta_from_y

//_____________________________________________________________________________
void TGenUPCJpsiFlat::put_tx_track(std::ostringstream &tx, int ipart) {

  //utility function for Starlight .tx format

  tx << "TRACK:  " << fPdgDat->ConvertPdgToGeant3(fPart[ipart]->GetPdgCode()) << " ";
  tx << fPart[ipart]->Px() << " " << fPart[ipart]->Py() << " " << fPart[ipart]->Pz();
  tx << " " << fNtx << " " << ipart << " 0 " << fPart[ipart]->GetPdgCode() << std::endl;

}//put_tx_track

















