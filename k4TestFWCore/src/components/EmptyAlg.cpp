#include "EmptyAlg.h"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include <vector>

#include "edm4hep/RecoParticleRefData.h"

DECLARE_COMPONENT(EmptyAlg)

EmptyAlg::EmptyAlg(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  // declareProperty("ReconstructedParticles", m_e4hhandle, "Generated particles collection (input)");
}

StatusCode EmptyAlg::initialize() {
  info() << "start EmptyAlg::initialize()\n";

  TFile* fileIn = new TFile("/eos/experiment/fcc/ee/generation/DelphesEvents/fcc_tmp/p8_ee_Ztautau_ecm91_EvtGen_Tau2MuGamma/events_001720714.root","READ");
  TTree* treeIn = (TTree*)fileIn->Get("events");
  // events *event = new events(treeIn);

  int nbranches = int(treeIn->GetNbranches());
  int nentries = int(treeIn->GetEntries());

  // treeIn->Print();

  // treeIn->SetBranchAddress("ReconstructedParticles", &elem1);
  std::vector<edm4hep::ReconstructedParticleData> test1;
  std::vector<edm4hep::ReconstructedParticleData>* ptest1 = &test1;

  info() << "vector size1: " << test1.size() << "\n";

  TBranch* branch = treeIn->GetBranch("ReconstructedParticles");
  branch->SetAddress(&ptest1);
  branch->GetEntry(0);

  info() << "vector size2: " << test1.size() << "\n";

  for (auto& e : test1) {
    info() << "charge: " << e.charge << "\n";
    info() << "momentum.x: " << e.momentum.x << "\n";
    info() << "momentum.y: " << e.momentum.y << "\n";
    info() << "momentum.z: " << e.momentum.z << "\n";
    info() << "energy: " << e.energy << "\n";
    info() << "mass: " << e.mass << "\n";
    info() << "tracks_begin: " << e.tracks_begin << "\n";
    info() << "\n";
  }

  // for(int i=0; i < 5; ++i) {
  //   treeIn->GenEntry(i);
  //   test1->
  // }

  // std::vector<edm4hep::ReconstructedParticleData>* elem1;
  // treeIn->SetBranchAddress("ReconstructedParticles", &elem1);

  // for (int i=0; i < 2; ++i) {
  //   treeIn->Show(i);
  //   treeIn->GetEvent(i);
  //   // info() << "event1: " << elem1[0]->energy << "\n";
  //   // info() << "Charge: " << elem1.charge << "\n";
  //   // info() << "Mass: " << elem1.mass << "\n";
  // }


  // // for(int i = 0; i < nbranches; ++i)
  // for(int i = 0; i < 5; ++i)
  // {
  //   TBranch* theBranch = (TBranch *) (treeIn->GetListOfBranches())->At(i);
  //   theBranch->Print();
  // }

  // for(int i = 0; i < nentries; ++i)
  //   treeIn->GetEvent(i);
  // }

  delete fileIn;

  return GaudiAlgorithm::initialize();
  // return StatusCode::SUCCESS;
}

StatusCode EmptyAlg::execute() {
  info() << "EmptyAlg::execute before\n";

  info() << "EmptyAlg::execute2 after\n";

  return StatusCode::SUCCESS;
}

StatusCode EmptyAlg::finalize() {
  info() << "EmptyAlg::finalize()\n";
  return StatusCode::SUCCESS;
}
