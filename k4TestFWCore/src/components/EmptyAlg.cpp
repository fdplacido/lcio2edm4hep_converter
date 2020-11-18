#include "EmptyAlg.h"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include <vector>

#include "edm4hep/RecoParticleRefData.h"

#include <string>
#include "lcio.h"
#include "IO/LCWriter.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/MCParticleImpl.h"
#include "IMPL/ReconstructedParticleImpl.h"
#include "IMPL/TrackImpl.h"
#include "IMPL/ParticleIDImpl.h"

#include <ctime>
#include <chrono>

DECLARE_COMPONENT(EmptyAlg)

EmptyAlg::EmptyAlg(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  // declareProperty("ReconstructedParticles", m_e4hhandle, "Generated particles collection (input)");
}

StatusCode EmptyAlg::initialize() {
  info() << "start EmptyAlg::initialize()\n";

  TFile* fileIn = new TFile("/eos/experiment/fcc/ee/generation/DelphesEvents/fcc_tmp/p8_ee_Ztautau_ecm91_EvtGen_Tau2MuGamma/events_001720714.root","READ");
  TTree* treeIn = (TTree*)fileIn->Get("events");
  // events *event = new events(treeIn);

  // int nbranches = int(treeIn->GetNbranches());
  // int nentries = int(treeIn->GetEntries());

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


  // LCIO

  const std::string output_filename {"output.slcio"};
  IO::LCWriter* my_lcwriter = nullptr;
  IMPL::LCCollectionVec* my_evtSumCol = nullptr;
  const std::string detector_name {"ToyTracker"};

  if (!output_filename.empty()) {
    my_lcwriter = lcio::LCFactory::getInstance()->createLCWriter();
    my_lcwriter->open(output_filename , lcio::LCIO::WRITE_NEW);
    my_evtSumCol = new lcio::LCCollectionVec(lcio::LCIO::LCGENERICOBJECT) ;
  }

  // loop over events
  if( my_evtSumCol ){
    for (int i=0; i<3; ++i) {
      auto evt = new lcio::LCEventImpl;
      evt->setEventNumber(i);
      evt->setDetectorName(detector_name);
      evt->setRunNumber(1);
      const auto p1 = (std::chrono::system_clock::now().time_since_epoch()).count();
      evt->setTimeStamp(p1);

      auto* recops = new lcio::LCCollectionVec(lcio::LCIO::RECONSTRUCTEDPARTICLE);
      auto* tracks = new lcio::LCCollectionVec(lcio::LCIO::TRACK);

      // loop over reconstructed particles
      for (int i=0; i<2; ++i) {
        auto* recp = new lcio::ReconstructedParticleImpl;
        recp->setCharge(2.2);
        double m[3] = {1.1, 2.2, 3.3};
        recp->setMomentum(m);
        recp->setEnergy(5.5);

        // if
        auto* track = new lcio::TrackImpl;

        track->setD0(1.1);
        track->setPhi(2.2);
        track->setOmega(3.3);
        track->setZ0(4.4);
        track->setTanLambda(5.5);
        track->subdetectorHitNumbers().resize(50);

        std::array<float, 15> cov;
        // float *cov;
        cov[0]  = 1;
        cov[2]  = 1;
        cov[5]  = 1;
        cov[9]  = 1;
        cov[14] = 1;

        track->setCovMatrix(cov.data());

        tracks->addElement(track);

        recp->addTrack(track);
        recops->addElement(recp);
      }

      evt->addCollection(tracks, lcio::LCIO::TRACK);
      evt->addCollection(recops, lcio::LCIO::RECONSTRUCTEDPARTICLE);

      // event.setEventNumber( iEvent )
      // event.setDetectorName( str(detectorName ))
      // event.setRunNumber( 1 )
      // event.setTimeStamp( int( time() * 1000000000. ) )

      // tracks = IMPL.LCCollectionVec(EVENT.LCIO.TRACK)
      // recops = IMPL.LCCollectionVec(EVENT.LCIO.RECONSTRUCTEDPARTICLE)
    }

  }

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
