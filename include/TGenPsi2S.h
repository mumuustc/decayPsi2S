#ifndef TGenPsi2S_h
#define TGenPsi2S_h

class TPythia8Decayer;
class TDatabasePDG;
class TLorentzVector;
class TClonesArray;
class TDecayPolarized;
class TParticle;
class TFile;
class TTree;

#include <vector>
#include <fstream>
#include "Rtypes.h"

class TGenPsi2S {

public:

  TGenPsi2S(const std::string& inp, const std::string& outp, int nev);
  ~TGenPsi2S();

  void SetEtaRange(double etamin, double etamax);
  void EventLoop();

private:

  bool PolarizedJpsi();

  void KeepFinalOnly();

  bool AcceptDecay();
  bool AcceptParticle(int idx);

  bool LoadInputEvent(TLorentzVector& vgen);
  void LoadParticle(TLorentzVector& pvec, const std::string& line);

  void WriteStarlight();
  void PutTxTrack(std::ostringstream &tx, unsigned int ipart);

  std::ifstream fInp; // input file
  unsigned long fNevt; // number of events to process

  std::ofstream fTxOut; //output in Starlight .tx format
  std::string fEvtline; // event line
  std::string fVtxline; // vertex line
  unsigned long fNtx; //output events to Starlight .tx format

  TDatabasePDG *fPdgDat; // PDG database
  TPythia8Decayer *fDec; // Pythia8 decayer
  TClonesArray *fPart; // clones array for decay products
  TDecayPolarized *fPol; // polarized J/psi decayer

  std::vector<const TParticle*> fVecPol; // all psi(2S) decay products with J/psi polarized decay

  bool fUseEta; // flag to use eta interval
  double fEtaMin; // minimal pseudorapidity
  double fEtaMax; // maximal pseudorapidity

  TFile *fRootOut; // output ROOT file
  TTree *jGenTree; // output tree with J/psi kinematics
  Double_t jGenPt, jGenPt2; // pT and pT^2
  Double_t jGenY; // rapidity
  Double_t jGenPhi; // azimuthal angle

};//TGenPsi2S

#endif














