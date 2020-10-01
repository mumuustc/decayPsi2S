#ifndef TDecayPolarized_h
#define TDecayPolarized_h

// vector meson two-body polarized decays,
// inspired by ALICE AliDecayerPolarized
// by Jaroslav Adam

class TParticle;
class TLorentzVector;
class TF1;
class TRandom3;

class TDecayPolarized {

public:

  TDecayPolarized(Int_t pdg=11, Double_t alpha=1);
  ~TDecayPolarized();

  void Generate(const TLorentzVector &vm);
  const TParticle* GetDecay(Int_t idx) const;

private:

  TF1 *fPol;  // decay products angular distribution
  TRandom3 *fRand; // random generator for azimuthal angles
  Double_t fMass; // mass of decayed particle
  Int_t fPdg; // PDG of decayed particle
  std::vector<TParticle> fVec; // decay products

};

#endif

