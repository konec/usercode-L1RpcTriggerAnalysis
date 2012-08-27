//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jun 12 14:50:31 2012 by ROOT version 5.32/00
// from TTree tree/tree
// found on file: nt0_all.root
//////////////////////////////////////////////////////////

#ifndef makePressureHumidityGraph_h
#define makePressureHumidityGraph_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TGraphErrors.h>

// Header file for the classes stored in the TTree if any.
#include <TObjString.h>
#include <TObject.h>
#include <TTimeStamp.h>

#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>

// Fixed size dimensions of array or collections stored in the TTree if any.

class makePressureHumidityGraph {
public :
   TChain          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   //TObjString      *DPNAME;
   UInt_t          fUniqueID;
   UInt_t          fBits;
   TString         fString;
   Double_t        VALUE;
   Double_t        RUNNUMBER;
   //TTimeStamp      *CHANGE_DATE;
   Int_t           fSec;
   Int_t           fNanoSec;
   //TTimeStamp      *STARTTIME;
   //   Int_t           fSec;
   //   Int_t           fNanoSec;
   //TTimeStamp      *STOPTIME;
   //   Int_t           fSec;
   //   Int_t           fNanoSec;

   // List of branches
   TBranch        *b_DPNAME_fUniqueID;   //!
   TBranch        *b_DPNAME_fBits;   //!
   TBranch        *b_DPNAME_fString;   //!
   TBranch        *b_VALUE;   //!
   TBranch        *b_RUNNUMBER;   //!
   TBranch        *b_CHANGE_DATE_fSec;   //!
   TBranch        *b_CHANGE_DATE_fNanoSec;   //!
   TBranch        *b_STARTTIME_fSec;   //!
   TBranch        *b_STARTTIME_fNanoSec;   //!
   TBranch        *b_STOPTIME_fSec;   //!
   TBranch        *b_STOPTIME_fNanoSec;   //!

   //   makePressureHumidityGraph(TTree *tree=0);
   makePressureHumidityGraph(TChain *tree,  
		  const char *finpRootfile=NULL, // WBM rootutple
		  const char *foutPressurePrefix="dummy_pressure",
		  const char *foutHumidityPrefix="dummy_humidity");
   ~makePressureHumidityGraph();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TChain *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

 private:
   const char *foutPressurePrefix; // output text/root file name prefix
   const char *foutHumidityPrefix; // output text/root file name prefix
   // assumed typical p, rH measuremnt precision:
   const double pressure_accuracy; // pressure in millibars
   const double humidity_accuracy; // relative humidity in percents [0-100]%
};

#endif

#ifdef makePressureHumidityGraph_cxx
makePressureHumidityGraph::makePressureHumidityGraph(TChain *tree,  
			       const char *inpRootfileList,
			       const char *outPressurePrefix,
			       const char *outHumidityPrefix) 
  : fChain(0), 
    foutPressurePrefix(outPressurePrefix), 
    foutHumidityPrefix(outHumidityPrefix),
    pressure_accuracy(0.1), // resolution = 0.1 mbar
    humidity_accuracy(0.1)  // resolution = 0.1 percent
{
  // if parameter tree is zero connect the rootfile
  // used to generate this class and read the TTree.
  if (tree == 0 && inpRootfileList && strlen(inpRootfileList)>0 ) {
    TChain *newtree = new TChain("tree");
    std::ifstream inp;
    inp.open(inpRootfileList, std::ifstream::in);
    while(inp.good()) {
      TString name;
      inp >> name;
      if(name=="") continue;
      newtree->AddFile(name.Data());
    };
    inp.close();
    Init(newtree);
  } else {
    Init(tree);
  }
}

makePressureHumidityGraph::~makePressureHumidityGraph()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t makePressureHumidityGraph::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t makePressureHumidityGraph::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void makePressureHumidityGraph::Init(TChain *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("fUniqueID", &fUniqueID, &b_DPNAME_fUniqueID);
   fChain->SetBranchAddress("fBits", &fBits, &b_DPNAME_fBits);
   fChain->SetBranchAddress("fString", &fString, &b_DPNAME_fString);
   fChain->SetBranchAddress("VALUE", &VALUE, &b_VALUE);
   fChain->SetBranchAddress("RUNNUMBER", &RUNNUMBER, &b_RUNNUMBER);
   fChain->SetBranchAddress("fSec", &fSec, &b_CHANGE_DATE_fSec);
   fChain->SetBranchAddress("fNanoSec", &fNanoSec, &b_CHANGE_DATE_fNanoSec);
//    fChain->SetBranchAddress("fSec", &fSec, &b_STARTTIME_fSec);
//    fChain->SetBranchAddress("fNanoSec", &fNanoSec, &b_STARTTIME_fNanoSec);
//    fChain->SetBranchAddress("fSec", &fSec, &b_STOPTIME_fSec);
//    fChain->SetBranchAddress("fNanoSec", &fNanoSec, &b_STOPTIME_fNanoSec);
   Notify();
}

Bool_t makePressureHumidityGraph::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void makePressureHumidityGraph::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t makePressureHumidityGraph::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef makePressureHumidityGraph_cxx
