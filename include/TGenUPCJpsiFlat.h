#ifndef TGenUPCJpsiFlat_h
#define TGenUPCJpsiFlat_h

#include "TGenerator.h"

#include <fstream>

class TRandom3;
class TDecayPolarized;
class TDatabasePDG;

class TGenUPCJpsiFlat : public TGenerator {

public:

  TGenUPCJpsiFlat(Double_t pt2max, Double_t etamin, Double_t etamax);
  ~TGenUPCJpsiFlat();

  void GenerateEvent();
  Int_t ImportParticles(TClonesArray *particles, Option_t *opt=0x0);
  void WriteStarlight();


private:

  Double_t eta_from_y(Double_t y, Double_t pT);
  void put_tx_track(std::ostringstream &tx, int ipart);

  Double_t fPt2Max; // maximal pT^2
  Double_t fEtaMin, fEtaMax, fEtaRange; // particles pseudorapidity range
  Double_t fPhiMin, fPhiRange; // azimuthal angle minimum and range
  Double_t fMass; // PDG J/psi mass

  TDatabasePDG *fPdgDat;

  TRandom3 *fRand; // random generator

  TDecayPolarized *fDec; // implements polarized J/psi decays
  std::vector<const TParticle*> fPart; // storage for pointers to decayed particles

  std::ofstream fTxOut; //output in Starlight .tx format
  std::string evtline[2]; // event line
  std::string vtxline; // vertex line
  unsigned long fNtx; //output events to Starlight .tx format

};//TGenUPCJpsiFlat

#endif

