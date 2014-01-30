//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Oct  4 09:25:41 2013 by ROOT version 5.34/07
// from TTree efficiencyTree/efficiencyTree
// found on file: /home/akalinow/scratch0/OverlapTrackFinder/job_4_ana/EfficiencyTree.root
//////////////////////////////////////////////////////////

#ifndef efficiencyTree_h
#define efficiencyTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <TObject.h>

// Fixed size dimensions of array or collections stored in the TTree if any.
const Int_t kMaxl1ObjectsOtf = 1;
const Int_t kMaxl1ObjectsGmt = 3;

class efficiencyTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Float_t         pt;

   // List of branches
   TBranch        *b_Events_pt;   //!

   efficiencyTree(TTree *tree=0);
   virtual ~efficiencyTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef efficiencyTree_cxx
efficiencyTree::efficiencyTree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/home/akalinow/scratch0/OverlapTrackFinder/job_4_ana/EfficiencyTree.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/home/akalinow/scratch0/OverlapTrackFinder/job_4_ana/EfficiencyTree.root");
      }
      f->GetObject("efficiencyTree",tree);

   }
   Init(tree);
}

efficiencyTree::~efficiencyTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t efficiencyTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t efficiencyTree::LoadTree(Long64_t entry)
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

void efficiencyTree::Init(TTree *tree)
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

  tree->SetBranchAddress("pt", &pt, &b_Events_pt);


   //////////
   tree->SetBranchStatus("*",0);
   tree->SetBranchStatus("pt",1); 
   
   int ii = 10;
   tree->Show(ii);
   b_Events_pt->GetEntry(ii);
   std::cout<<" pt: "<<pt<<" "<<std::endl;
   ///////////
   
   Notify();
}

Bool_t efficiencyTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void efficiencyTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t efficiencyTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef efficiencyTree_cxx
