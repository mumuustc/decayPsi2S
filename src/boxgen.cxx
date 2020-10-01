
//C++ headers
#include <iostream>

//ROOT headers
#include "TClonesArray.h"
#include "TParticle.h"
#include "TFile.h"
#include "TTree.h"

//local headers
#include "TGenUPCJpsiFlat.h"

using namespace std;

//_____________________________________________________________________________
int main(int argc, char* argv[]) {

  //configure the generator
  //TGenUPCJpsiFlat gen(0.14, -1.2, 1.2); // max pT^2 and eta range
  TGenUPCJpsiFlat gen(0.01, -1.2, 1.2); // max pT^2 and eta range
  TClonesArray *particles = new TClonesArray("TParticle");

  //Int_t nev = 120;
  int nev = 6e6;
  int nprint = nev/12;

  //ROOT output
  TFile *outfile = TFile::Open("output.root", "recreate");
  Double_t d0pT, d0eta, d0phi, d1pT, d1eta, d1phi;
  Double_t mrec, pTrec, pT2rec, yrec;
  Double_t d0cosTheta_hx, d0phi_hx, d1cosTheta_hx, d1phi_hx;
  TTree *bgen_tree = new TTree("bgen_tree", "bgen_tree");
  bgen_tree ->Branch("d0pT", &d0pT, "d0pT/D");
  bgen_tree ->Branch("d0eta", &d0eta, "d0eta/D");
  bgen_tree ->Branch("d0phi", &d0phi, "d0phi/D");
  bgen_tree ->Branch("d1pT", &d1pT, "d1pT/D");
  bgen_tree ->Branch("d1eta", &d1eta, "d1eta/D");
  bgen_tree ->Branch("d1phi", &d1phi, "d1phi/D");
  bgen_tree ->Branch("mrec", &mrec, "mrec/D");
  bgen_tree ->Branch("pTrec", &pTrec, "pTrec/D");
  bgen_tree ->Branch("pT2rec", &pT2rec, "pT2rec/D");
  bgen_tree ->Branch("yrec", &yrec, "yrec/D");
  bgen_tree ->Branch("d0cosTheta_hx", &d0cosTheta_hx, "d0cosTheta_hx/D");
  bgen_tree ->Branch("d0phi_hx", &d0phi_hx, "d0phi_hx/D");
  bgen_tree ->Branch("d1cosTheta_hx", &d1cosTheta_hx, "d1cosTheta_hx/D");
  bgen_tree ->Branch("d1phi_hx", &d1phi_hx, "d1phi_hx/D");

  //event loop
  for(int i=0; i<nev; i++) {

    if (i%nprint == 0 && i != 0) {
      cout << "processed " << i+1 << " events" << endl;
    }

    //make the decay
    gen.GenerateEvent();
    gen.WriteStarlight();
    gen.ImportParticles(particles);

    //retrieve generated particles
    const TParticle *vmDaughter0 = dynamic_cast<TParticle*>( particles->At(0) );
    const TParticle *vmDaughter1 = dynamic_cast<TParticle*>( particles->At(1) );

    //cout << vmDaughter0->Eta() << " " << vmDaughter1->Eta() << endl;

    //decay particles in output tree
    d0pT = vmDaughter0->Pt();
    d0eta = vmDaughter0->Eta();
    d0phi = vmDaughter0->Phi();
    d1pT = vmDaughter1->Pt();
    d1eta = vmDaughter1->Eta();
    d1phi = vmDaughter1->Phi();

    //decay particles Lorentz vectors
    TLorentzVector d0vec, d1vec;
    vmDaughter0->Momentum(d0vec);
    vmDaughter1->Momentum(d1vec);

    //verify vector meson reconstruction from the decay
    TLorentzVector vmrec = d0vec + d1vec;
    mrec = vmrec.M();
    pTrec = vmrec.Pt();
    pT2rec = pTrec*pTrec;
    yrec = vmrec.Rapidity();

    //angular distribution in J/psi rest frame
    TVector3 bvec = vmrec.BoostVector();
    d0vec.Boost(-bvec.x(), -bvec.y(), -bvec.z());
    d1vec.Boost(-bvec.x(), -bvec.y(), -bvec.z());
    d0cosTheta_hx = d0vec.CosTheta();
    d0phi_hx = d0vec.Phi();
    d1cosTheta_hx = d1vec.CosTheta();
    d1phi_hx = d1vec.Phi();

    //fill output tree
    bgen_tree->Fill();

  }//event loop

  bgen_tree->Write();
  outfile->Close();

  return 0;

}//main


















