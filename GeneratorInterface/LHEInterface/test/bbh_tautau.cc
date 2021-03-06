// system include files
// http://cmslxr.fnal.gov/lxr/source/RecoJets/JetAlgorithms/src/JetIDHelper.cc?v=CMSSW_3_3_6
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
//
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
//#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/Utilities/interface/InputTag.h"
// gen particles
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/WeightsInfo.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
// parton jets
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
// MC info
//#include "CLHEP/Vector/LorentzVector.h"
//#include "CLHEP/Units/GlobalPhysicalConstants.h"
#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
//#include "CLHEP/HepPDT/DefaultConfig.hh"
//
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "Math/GenVector/VectorUtil.h"
#include "Math/GenVector/PxPyPzE4D.h"
#include "DataFormats/Math/interface/deltaR.h"
//double dR = deltaR( c1, c2 );
//
// root
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
//
using namespace std;
using namespace reco;

//
// class declaration
//

class bbh_tautau : public edm::EDAnalyzer {
public:
  explicit bbh_tautau(const edm::ParameterSet&);
  ~bbh_tautau();


private:
  //      virtual void beginJob(const edm::EventSetup&) ;
  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;

  // ----------member data ---------------------------
  // output root file
  string fOutputFileName ;
  edm::InputTag genjets04_Src; 
  // names of modules, producing object collections
  //  edm::InputTag partonjetsSrc; 
  // variables to store in ntpl
  double pth22, pth44, pth62, Hmass;
  double ptb1, etab1, phib1;
  double ptb2, etab2, phib2;
  double ptmu1, etamu1, phimu1;
  double ptmu2, etamu2, phimu2;  

  std::vector<double> *EtaJ;
  std::vector<double> *PhiJ;
  std::vector<double> *pTJ;
  std::vector<double> *bjet;
  std::vector<double> *evtweight;

  //
  TFile*      hOutputFile ;
  TTree*      t1;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

// ------------ method called once each job just before starting event loop  ------------
void 
//bbh_tautau::beginJob(const edm::EventSetup&)
bbh_tautau::beginJob()
{
  using namespace edm;

  // creating a simple tree

  hOutputFile   = new TFile( fOutputFileName.c_str(), "RECREATE" ) ;

  EtaJ         = new std::vector<double>();
  PhiJ         = new std::vector<double>();
  pTJ          = new std::vector<double>();
  bjet         = new std::vector<double>();
  evtweight    = new std::vector<double>();

  t1 = new TTree("t1","analysis tree");

  t1->Branch("pth22",&pth22,"pth22/D");
  t1->Branch("pth44",&pth44,"pth44/D");
  t1->Branch("pth62",&pth62,"pth62/D");
  t1->Branch("Hmass",&Hmass,"Hmass/D");

  t1->Branch("ptb1",&ptb1,"ptb1/D");
  t1->Branch("etab1",&etab1,"etab1/D");
  t1->Branch("phib1",&phib1,"phib1/D");
  //
  t1->Branch("ptb2",&ptb2,"ptb2/D");
  t1->Branch("etab2",&etab2,"etab2/D");
  t1->Branch("phib2",&phib2,"phib2/D");
  //
  t1->Branch("ptmu1",&ptmu1,"ptmu1/D");
  t1->Branch("etamu1",&etamu1,"etamu1/D");
  t1->Branch("phimu1",&phimu1,"phimu1/D");
  //
  t1->Branch("ptmu2",&ptmu2,"ptmu2/D");
  t1->Branch("etamu2",&etamu2,"etamu2/D");
  t1->Branch("phimu2",&phimu2,"phimu2/D");

  t1->Branch("EtaJ","vector<double>",&EtaJ);
  t1->Branch("PhiJ","vector<double>",&PhiJ);
  t1->Branch("pTJ" ,"vector<double>",&pTJ);
  t1->Branch("bjet" ,"vector<double>",&bjet);
  t1->Branch("evtweight","vector<double>",&evtweight);

  return ;
}

// ------------ method called once each job just after ending the event loop  ------------
void 
bbh_tautau::endJob() {

  hOutputFile->Write() ;
  hOutputFile->Close() ;

  return ;
}

//
// constructors and destructor
//
bbh_tautau::bbh_tautau(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
  using namespace edm;
  // 
  // get name of output file with histogramms
  fOutputFileName = iConfig.getUntrackedParameter<string>("HistOutFile");
  //
  // get gen jets
  genjets04_Src   = iConfig.getParameter<edm::InputTag>("gen_jets_ak4");
}


bbh_tautau::~bbh_tautau()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
bbh_tautau::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  //
  pth22    = 0.; 
  pth44    = 0.; 
  pth62    = 0.; 
  Hmass    = 0.;

  ptb1    = 0.; 
  etab1   = 0.; 
  phib1   = 0.;
  ptb2    = 0.; 
  etab2   = 0.; 
  phib2   = 0.;
  ptmu1   = 0.; 
  etamu1  = 0.; 
  phimu1  = 0.;
  ptmu2   = 0.; 
  etamu2  = 0.; 
  phimu2  = 0.;

  EtaJ->clear();
  PhiJ->clear();
  pTJ->clear();
  bjet->clear();
  evtweight->clear();

  edm::Handle<GenEventInfoProduct> genEvt;
  iEvent.getByLabel("generator",genEvt);

  // event weight
  //  double weightevt=genEvt->weight();
  //  cout <<"  weightevt = " << weightevt << endl;

  
  //  for( unsigned int i = 0; i < 50; i++) {
     evtweight->push_back(1.0);
  //  }

  edm::Handle<LHEEventProduct > lheEvt;
  iEvent.getByLabel("source",lheEvt);
  if(lheEvt.isValid()){
    /*
    int nup = lheEvt->hepeup().NUP;
    for( int i = 0; i < nup; i++) {
      int lhep_id = lheEvt->hepeup().IDUP[i];
      lhef::HEPEUP::FiveVector p = lheEvt->hepeup().PUP[i];
      double ppx = p[0];
      double ppy = p[1];
      if(abs(lhep_id) == 21 && i == nup-1) {
	ptg = sqrt(ppx*ppx + ppy*ppy);
      }
    }
    */
    std::vector<gen::WeightsInfo> weights = lheEvt->weights();
    if(weights.size() != 0) {
      // cout <<"weights.size() !=0"<<endl;
      evtweight->clear();}
    for( unsigned int i = 0; i < weights.size(); i++) {
      gen::WeightsInfo lhew = weights[i];
      evtweight->push_back(lhew.wgt);
      // cout <<"  weights -> i = " << lhew.id <<"  weight = " << lhew.wgt << endl;
    }
  }
    // cout <<" ==> event weight = " << evtweight << endl;

 // gen jets 04
  edm::Handle<GenJetCollection> genjets04;
  iEvent.getByLabel(genjets04_Src, genjets04);

  if (!genjets04.isValid()) {return;}

  if( genjets04->size() != 0) {

    for(GenJetCollection::const_iterator parton_or_gen_jet = genjets04->begin();  
	parton_or_gen_jet != genjets04->end(); ++parton_or_gen_jet ) { 
      if( ( parton_or_gen_jet->pt() > 20. ) && ( fabs(parton_or_gen_jet->eta()) ) < 4.7 ) {
	EtaJ->push_back(parton_or_gen_jet->eta());
	PhiJ->push_back(parton_or_gen_jet->phi());
	pTJ->push_back(parton_or_gen_jet->pt());
      }
    }
  }

  edm::Handle<reco::GenParticleCollection> genparticles;
  iEvent.getByLabel("genParticles", genparticles);
  // cout<<"Jet size "<<pTJ->size()<<endl;
  // define the jet 04 flavour
  for (unsigned int j = 0; j < pTJ->size(); ++j) {
    double ib = 0;
    // cout<<"Genparticles size "<<genparticles->size()<<endl;
    for( size_t i = 0; i < genparticles->size(); i++) {
      const reco::GenParticle & p = (*genparticles)[i];
      if( abs(p.pdgId()) == 511 ||
	  abs(p.pdgId()) == 521 ||
	  abs(p.pdgId()) == 531 ||
	  abs(p.pdgId()) == 541 ||
	  abs(p.pdgId()) == 5112 ||	    
	  abs(p.pdgId()) == 5122 ||	    
	  abs(p.pdgId()) == 5132 ||	    
	  abs(p.pdgId()) == 5142 ||	    
	  abs(p.pdgId()) == 5212 ||	    
	  abs(p.pdgId()) == 5222 ||	    
	  abs(p.pdgId()) == 5232 ||	    
	  abs(p.pdgId()) == 5242 ) {
	double djb = deltaR((*EtaJ)[j],(*PhiJ)[j],p.eta(),p.phi());
	if(djb < 0.4) {
    // cout<<"Jet "<<j<<" found with pdgID "<<p.pdgId()<<" and DeltaR "<<djb<<endl;
	  ib = 1;
	  continue;
	}
      }	    
    }
    bjet->push_back(ib);
  }

  //  edm::Handle<reco::GenParticleCollection> genparticles;
  //  iEvent.getByLabel("genParticles", genparticles);
  //  cout <<"  ==> genparticles size = " << genparticles->size() << endl;

  math::XYZTLorentzVector  tauh1(0.,0.,0.,0.);
  math::XYZTLorentzVector  tauh2(0.,0.,0.,0.);

  for( size_t i = 0; i < genparticles->size(); i++)
    {
      const reco::GenParticle & p = (*genparticles)[i];

      int motherID = 0;
      int motherSt = 0;

      const Candidate * moth = p.mother();
      if(moth) {
	motherID = moth->pdgId();
	motherSt = moth->status();	
      }

      /*
      cout <<" i = " << i+1 <<" id = " << p.pdgId()
	   <<" status = " << p.status()
	   <<" ndouthers = " << p.numberOfDaughters()
	   <<" motherID = " << motherID 
	   <<" motherST = " << motherSt << endl;
      */

      // select tau_h decay products from h->2tau
      /*
      if(abs(p.pdgId()) != 11 && abs(p.pdgId()) != 12 && 
	 abs(p.pdgId()) != 13 && abs(p.pdgId()) != 14 &&
	 abs(p.pdgId()) != 15 && abs(p.pdgId()) != 16 && 
	 abs(motherID) == 15 && motherSt == 2)
	{  
	  math::XYZTLorentzVector tauh_c(p.px(),p.py(),p.pz(),p.p());
	  if(motherID ==  15) 
	    {
	      tauh1 += tauh_c;
	      //	      cout <<" tau- decay particle " << endl;
	    }
	  if(motherID == -15) 
	    {
	      tauh2 += tauh_c;
	      //	      cout <<" tau+ decay particle " << endl;
	    }
	}
      */

	if(p.pdgId() == 13 && (motherID == 25 || motherID == 36) && motherSt == 62) {
	ptmu1  = p.pt();
	etamu1 = p.eta();
	phimu1 = p.phi();
	//	cout <<" ->mu1 px= " << p.px() << endl; 
	}

	if(p.pdgId() == -13 && (motherID == 25 || motherID == 36) && motherSt == 62) {
	ptmu2  = p.pt();
	etamu2 = p.eta();
	phimu2 = p.phi();
	//	cout <<" ->mu2 px= " << p.px() << endl; 
	}
      // select b quarks
      if(p.pdgId() == 5 && p.status() == 23) {
	ptb1  = p.pt();
	etab1 = p.eta();
	phib1 = p.phi();
	//	cout <<" -> b quark, pt = " << p.pt() <<" eta = " << p.eta() << endl; 
      }
      //
      if(p.pdgId() == -5 && p.status() == 23) {
	ptb2  = p.pt();
	etab2 = p.eta();
	phib2 = p.phi();
	//	cout <<" -> b~quark, pt = " << p.pt() <<" eta = " << p.eta() << endl; 
	//	cout <<" ->b2 px= " << p.px() << endl; 
      }

      if( p.pdgId() == 25 || p.pdgId() == 35 || p.pdgId() == 36 )
	{
	  //	  cout <<" Higgs id = " << p.pdgId() <<" status = " << fabs(p.status()) << endl;
	  if( fabs(p.status()) == 22) {pth22 = p.pt();}
	  if( fabs(p.status()) == 44) {pth44 = p.pt();}
	  if( fabs(p.status()) == 62) {
	    pth62 = p.pt();
	    Hmass = p.mass();
	  }
	}
    }

  // for taus
  /*
  ptmu1  = tauh1.Pt();
  etamu1 = tauh1.Eta();
  phimu1 = tauh1.Phi();
  
  ptmu2  = tauh2.Pt();
  etamu2 = tauh2.Eta();
  phimu2 = tauh2.Phi();
  */

  t1->Fill();
}

//define this as a plug-in
DEFINE_FWK_MODULE(bbh_tautau);
