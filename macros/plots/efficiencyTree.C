#define efficiencyTree_cxx
#include "efficiencyTree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void efficiencyTree::Loop(){

   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   fChain->SetBranchStatus("*",0);
   fChain->SetBranchStatus("pt",1); 

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      std::cout<<"pt: "<<pt<<std::endl;

   }
}
