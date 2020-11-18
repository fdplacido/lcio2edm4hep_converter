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

  TFile* input_file = new TFile("/eos/experiment/fcc/ee/generation/DelphesEvents/fcc_tmp/p8_ee_Ztautau_ecm91_EvtGen_Tau2MuGamma/events_001720714.root","READ");
  TTree* input_tree = (TTree*)input_file->Get("events");
  // events *event = new events(input_tree);

  // int nbranches = int(input_tree->GetNbranches());
  int nentries = int(input_tree->GetEntries());

  info() << "nentries: " << nentries << "\n";

  // input_tree->Print();

  // for(int i=0; i < 5; ++i) {
  //   input_tree->GenEntry(i);
  //   rp_vec->
  // }

  // std::vector<edm4hep::ReconstructedParticleData>* elem1;
  // input_tree->SetBranchAddress("ReconstructedParticles", &elem1);

  // for (int i=0; i < 2; ++i) {
  //   input_tree->Show(i);
  //   input_tree->GetEvent(i);
  //   // info() << "event1: " << elem1[0]->energy << "\n";
  //   // info() << "Charge: " << elem1.charge << "\n";
  //   // info() << "Mass: " << elem1.mass << "\n";
  // }


  // // for(int i = 0; i < nbranches; ++i)
  // for(int i = 0; i < 5; ++i)
  // {
  //   TBranch* theBranch = (TBranch *) (input_tree->GetListOfBranches())->At(i);
  //   theBranch->Print();
  // }

  // for(int i = 0; i < nentries; ++i)
  //   input_tree->GetEvent(i);
  // }  

  // LCIO stuff to write file
  const std::string output_filename {"output.slcio"};
  IO::LCWriter* my_lcwriter = nullptr;
  // IMPL::LCCollectionVec* my_evtSumCol = nullptr;
  const std::string detector_name {"ToyTracker"};

  if (!output_filename.empty()) {
    my_lcwriter = lcio::LCFactory::getInstance()->createLCWriter();
    my_lcwriter->open(output_filename , lcio::LCIO::WRITE_NEW);
    // my_evtSumCol = new lcio::LCCollectionVec(lcio::LCIO::LCGENERICOBJECT) ;
  }    

  // loop over the events
  for (int e = 0; e < nentries; ++e) {

    // process just first 3
    if (e == 3) break;

    // input_tree->SetBranchAddress("ReconstructedParticles", &elem1);
    std::vector<edm4hep::ReconstructedParticleData> rp_vec;
    std::vector<edm4hep::ReconstructedParticleData>* prp_vec = &rp_vec;

    TBranch* rp_branch = input_tree->GetBranch("ReconstructedParticles");
    rp_branch->SetAddress(&prp_vec);
    rp_branch->GetEntry(e); // here is the loop over the events

    std::vector<edm4hep::TrackState> eft_vec;
    std::vector<edm4hep::TrackState>* peft_vec = &eft_vec;

    TBranch* eft_branch = input_tree->GetBranch("EFlowTrack_1");
    eft_branch->SetAddress(&peft_vec);
    eft_branch->GetEntry(e);

    info() << "number of rec particles: " << rp_vec.size() << "\n";

    // This would be done for every event
    auto event = new lcio::LCEventImpl;
    event->setEventNumber(e);
    event->setDetectorName(detector_name);
    event->setRunNumber(1);
    const auto p1 = (std::chrono::system_clock::now().time_since_epoch()).count();
    event->setTimeStamp(p1);

    auto* recops = new lcio::LCCollectionVec(lcio::LCIO::RECONSTRUCTEDPARTICLE);
    auto* tracks = new lcio::LCCollectionVec(lcio::LCIO::TRACK);

    // loop over EDM4hep particles
    // for (auto& rp : rp_vec) {
    for(uint rp=0; rp < rp_vec.size(); ++rp) {

      // print EDM4hep particle for checks
      info() << "Particle Info:\n";
      info() << "charge: " << rp_vec.at(rp).charge << "\n";
      info() << "momentum.x: " << rp_vec.at(rp).momentum.x << "\n";
      info() << "momentum.y: " << rp_vec.at(rp).momentum.y << "\n";
      info() << "momentum.z: " << rp_vec.at(rp).momentum.z << "\n";
      info() << "energy: " << rp_vec.at(rp).energy << "\n";
      info() << "mass: " << rp_vec.at(rp).mass << "\n";
      info() << "tracks_begin: " << rp_vec.at(rp).tracks_begin << "\n";
      info() << "\n";

      auto* recp = new lcio::ReconstructedParticleImpl;
      recp->setCharge(rp_vec.at(rp).charge);
      double m[3] = {rp_vec.at(rp).momentum.x, rp_vec.at(rp).momentum.y, rp_vec.at(rp).momentum.z};
      recp->setMomentum(m);
      recp->setEnergy(rp_vec.at(rp).energy);

      if (rp_vec.at(rp).tracks_begin < eft_vec.size()) {
        auto* track = new lcio::TrackImpl;
        auto trkind = rp_vec.at(rp).tracks_begin;

        track->setD0(eft_vec.at(trkind).D0);
        track->setPhi(eft_vec.at(trkind).phi);
        track->setOmega(eft_vec.at(trkind).omega);
        track->setZ0(eft_vec.at(trkind).Z0);
        // track->setTanLambda(eft_vec.at(trkind).tanLamda);
        track->subdetectorHitNumbers().resize(50);

        std::array<float, 15> cov;
        // float *cov;
        cov[0]  = rp_vec.at(rp).covMatrix[0];
        cov[2]  = rp_vec.at(rp).covMatrix[5];
        cov[5]  = rp_vec.at(rp).covMatrix[9];
        cov[9]  = rp_vec.at(rp).covMatrix[12];
        cov[14] = rp_vec.at(rp).covMatrix[14];

        track->setCovMatrix(cov.data());

        tracks->addElement(track);

        recp->addTrack(track);
        recops->addElement(recp);
      }
    }

    event->addCollection(tracks, lcio::LCIO::TRACK);
    event->addCollection(recops, lcio::LCIO::RECONSTRUCTEDPARTICLE);

    my_lcwriter->writeEvent(event);
  }

  // clean up ?
  my_lcwriter->flush();
  my_lcwriter->close();
  delete input_file;
  // delete recops;
  // delete tracks;
  // delete event;

  return GaudiAlgorithm::initialize();
  // return StatusCode::SUCCESS;
}

StatusCode EmptyAlg::execute() {
  info() << "EmptyAlg::execute()\n";
  return StatusCode::SUCCESS;
}

StatusCode EmptyAlg::finalize() {
  info() << "EmptyAlg::finalize()\n";
  return StatusCode::SUCCESS;
}
