
//c++ headers
#include <string>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <iomanip>
#include <sstream>

//ROOT headers
#include <TPythia8Decayer.h>
#include "TDatabasePDG.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"
#include "TParticle.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"

//local headers
#include "TDecayPolarized.h"
#include "TGenPsi2S.h"

using namespace std;
using namespace boost;

//_____________________________________________________________________________
TGenPsi2S::TGenPsi2S(const string& inp, const string& outp, int nev): fNevt(nev), fNtx(1),
  fUseEta(false), fEtaMin(0), fEtaMax(0), jGenPt(0), jGenPt2(0),
  jGenY(0), jGenPhi(0) {

  fInp.open(inp);

  fDec = new TPythia8Decayer();
  fDec->Init();

  fPdgDat = TDatabasePDG::Instance();

  fPart = new TClonesArray("TParticle");

  fPol = new TDecayPolarized(13, 1.);

  fTxOut.open(Form("%s.tx", outp.c_str()));
  fEvtline = "EVENT: ";
  fVtxline = "VERTEX: 0 0 0 0 1 0 0 ";

  fRootOut = new TFile(Form("%s.root", outp.c_str()), "recreate");
  jGenTree = new TTree("jGenTree", "jGenTree");
  jGenTree ->Branch("jGenPt", &jGenPt, "jGenPt/D");
  jGenTree ->Branch("jGenPt2", &jGenPt2, "jGenPt2/D");
  jGenTree ->Branch("jGenY", &jGenY, "jGenY/D");
  jGenTree ->Branch("jGenPhi", &jGenPhi, "jGenPhi/D");

}//TGenPsi2S

//_____________________________________________________________________________
TGenPsi2S::~TGenPsi2S() {

  jGenTree->Write();
  fRootOut->Close();
  delete fRootOut;

  fTxOut.close();
  fInp.close();

  delete fDec;

  fPart->Clear();
  delete fPart;

  delete fPol;

};//~TGenPsi2S

//_____________________________________________________________________________
void TGenPsi2S::SetEtaRange(double etamin, double etamax) {

  //set pseudorapidity interval for e+ and e- from J/psi decay

  fUseEta = true;

  fEtaMin = etamin;
  fEtaMax = etamax;

}//SetEtaRange

//_____________________________________________________________________________
void TGenPsi2S::EventLoop() {

  unsigned long iev = 0;
  unsigned long nprint = 5e5;
  unsigned long nreject = 0;

  //input event loop
  while(true) {

    if(iev > fNevt and fNevt != 0) break;

    //original psi(2S) event
    TLorentzVector vgen;
    if( !LoadInputEvent(vgen) ) break;

    //reject broken input events
    if( vgen.M() < 0.1 ) {

      cout << "TGenPsi2S: rejecting input event: ";
      cout << iev+1 << " ";
      cout << vgen.Pt() << " " << vgen.Rapidity() << " ";
      cout << vgen.M() << endl;

      ++nreject;

      continue;
    }

    //decay the psi(2S)
    while(true) {

      fPart->Clear();
      fDec->Decay(100443, &vgen);
      fDec->ImportParticles(fPart);

      if( AcceptDecay() ) break;
    }
    KeepFinalOnly();

    //polarized J/psi decay
    if( !PolarizedJpsi() ) continue;

    //write the output in .tx format
    WriteStarlight();

    //write J/psi kinematics in output tree
    jGenTree->Fill();

    /*
    cout.precision(4);
    cout << vgen.Pt() << " " << vgen.Rapidity() << " " << vgen.M() << endl;

    //decayer loop
    for(int i=0; i<fPart->GetEntries(); i++) {

      TParticle *part = dynamic_cast<TParticle*>( fPart->At(i) );

      cout << setw(3) << i << setw(7) << part->GetPdgCode() << setw(7) << fPdgDat->GetParticle(part->GetPdgCode())->GetName();
      cout << setw(3) << part->GetFirstDaughter() << setw(3) << part->GetLastDaughter();
      cout << setw(3) << part->GetNDaughters() << endl;

    }//decayer loop

    //cout << fVecPol.size() << " " << fPart->GetEntries() << endl;
    for(vector<const TParticle*>::const_iterator it = fVecPol.cbegin(); it != fVecPol.cend(); ++it) {
      cout << setw(5) << (*it)->GetPdgCode() << endl;
    }
    */

    iev++;

    if (iev != 0 and iev%nprint == 0) {
      cout << "processed " << iev << " events" << endl;
    }

  }//input event loop

  cout << "Rejected input events: " << nreject << endl;
  cout << "Events written: " << iev << endl;

}//EventLoop

//_____________________________________________________________________________
bool TGenPsi2S::PolarizedJpsi() {

  //polarized J/psi -> e+e- decay

  int idx;
  for(int i=0; i<fPart->GetEntries(); i++) {
    TParticle *part = dynamic_cast<TParticle*>( fPart->At(i) );

    if( part->GetPdgCode() == 443 ) {
      idx = i;
      break;
    }
  }

  //original J/psi Lorentz vector and removal from decay clones array
  TParticle *pjpsi = dynamic_cast<TParticle*>( fPart->At(idx) );
  TLorentzVector vjpsi;
  pjpsi->Momentum(vjpsi);
  fPart->RemoveAt(idx);
  fPart->Compress();

  //J/psi kinematics in output tree
  jGenPt = vjpsi.Pt();
  jGenPt2 = jGenPt*jGenPt;
  jGenY = vjpsi.Rapidity();
  jGenPhi = vjpsi.Phi();

  //generate the decay
  fPol->Generate(vjpsi);

  //store all psi(2S) decay products including polarized J/psi
  fVecPol.clear();
  fVecPol.resize( fPart->GetEntries() + 2 );

  fVecPol[0] = fPol->GetDecay(0);
  fVecPol[1] = fPol->GetDecay(1);

  for(int i=0; i<fPart->GetEntries(); i++) {
    fVecPol[i+2] = dynamic_cast<TParticle*>( fPart->At(i) );
  }

  if( !fUseEta ) return true;

  //evaluate the requested pseudorapidity interval

  double eta0 = fPol->GetDecay(0)->Eta();
  double eta1 = fPol->GetDecay(1)->Eta();

  if( eta0 < fEtaMin or eta0 > fEtaMax ) return false;
  if( eta1 < fEtaMin or eta1 > fEtaMax ) return false;

  //decay passed the pseudorapidity interval

  return true;

}//PolarizedJpsi

//_____________________________________________________________________________
void TGenPsi2S::KeepFinalOnly() {

  //keep only J/psi and final products of psi(2S) decay, also dileptons from J/psi
  //decay are removed since polarized decay will follow

  vector<int> to_remove;
  to_remove.reserve(fPart->GetEntries());

  for(int i=0; i<fPart->GetEntries(); i++) {
    TParticle *part = dynamic_cast<TParticle*>( fPart->At(i) );

    if( part->GetPdgCode() == 443 ) {
      to_remove.push_back( part->GetFirstDaughter() );
      to_remove.push_back( part->GetLastDaughter() );
      continue;
    }

    if( part->GetNDaughters() > 0 ) to_remove.push_back(i);
  }

  for(vector<int>::const_iterator it = to_remove.cbegin(); it != to_remove.cend(); ++it) {
    fPart->RemoveAt(*it);
  }

  fPart->Compress();

}//KeepFinalOnly

//_____________________________________________________________________________
bool TGenPsi2S::AcceptDecay() {

  //select J/psi dilepton decays

  int idx0=0, idx1=0;
  for(int i=0; i<fPart->GetEntries(); i++) {
    TParticle *part = dynamic_cast<TParticle*>( fPart->At(i) );

    if( part->GetPdgCode() == 443 ) {
      idx0 = part->GetFirstDaughter();
      idx1 = part->GetLastDaughter();
      break;
    }
  }

  if( idx0 <= 0 or idx1 <= 0 or TMath::Abs(idx1-idx0) != 1 ) return false;

  if( !AcceptParticle(idx0) or !AcceptParticle(idx1) ) return false;

  return true;

}//AcceptDecay

//_____________________________________________________________________________
bool TGenPsi2S::AcceptParticle(int idx) {

  TParticle *part = dynamic_cast<TParticle*>( fPart->At(idx) );

  int pdg = part->GetPdgCode();

  if( TMath::Abs(pdg) != 11 and TMath::Abs(pdg) != 13 ) return false;

  return true;

}//AcceptParticle

//_____________________________________________________________________________
bool TGenPsi2S::LoadInputEvent(TLorentzVector& vgen) {

  string line;

  //event and vertex lines
  getline(fInp, line);
  if( !fInp.good() ) return false;
  getline(fInp, line);

  //particle lines
  TLorentzVector v0, v1;
  getline(fInp, line);
  LoadParticle(v0, line);

  getline(fInp, line);
  LoadParticle(v1, line);

  vgen = v0 + v1;

  return true;

}//LoadInputEvent

//_____________________________________________________________________________
void TGenPsi2S::LoadParticle(TLorentzVector& pvec, const std::string& line) {

  //cout << line << endl;

  char_separator<char> sep(" ");
  tokenizer<char_separator<char>> tok(line, sep);
  tokenizer<char_separator<char>>::iterator trk_it=tok.begin();

  //particle momentum
  ++trk_it; ++trk_it;
  Double_t pxyz[3];
  for(int i=0; i<3; i++) pxyz[i] = stod( *(trk_it++) );

  //particle pdg
  for(int i=0; i<3; i++) ++trk_it;
  int pdg = stoi( *trk_it );

  //set particle Lorentz vector
  pvec.SetXYZM(pxyz[0], pxyz[1], pxyz[2], fPdgDat->GetParticle(pdg)->Mass());

}//LoadParticle

//_____________________________________________________________________________
void TGenPsi2S::WriteStarlight() {

  //write output in Starlight .tx format

  std::ostringstream tx;

  tx << fEvtline << fNtx << " " << fVecPol.size() << " 1" << endl;
  tx << fVtxline << fVecPol.size() << endl;
  for(unsigned int i=0; i<fVecPol.size(); i++) PutTxTrack(tx, i);

  fTxOut << tx.str();

  ++fNtx;

}//WriteStarlight

//_____________________________________________________________________________
void TGenPsi2S::PutTxTrack(ostringstream &tx, unsigned int ipart) {

  //utility function for Starlight .tx format

  tx << "TRACK:  " << fPdgDat->ConvertPdgToGeant3( fVecPol[ipart]->GetPdgCode() ) << " ";
  tx << fixed << fVecPol[ipart]->Px() << " ";
  tx << fixed << fVecPol[ipart]->Py() << " ";
  tx << fixed << fVecPol[ipart]->Pz() << " ";
  tx << fNtx << " " << ipart << " 0 ";
  tx << fVecPol[ipart]->GetPdgCode() << endl;

}//put_tx_track



















