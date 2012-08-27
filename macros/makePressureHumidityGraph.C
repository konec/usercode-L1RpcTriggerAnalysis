#define makePressureHumidityGraph_cxx
#include "makePressureHumidityGraph.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <map>

void makePressureHumidityGraph::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L makePressureHumidityGraph.C
//      Root > makePressureHumidityGraph t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch

  if (fChain == 0) return;
  
  Long64_t nentries = fChain->GetEntriesFast();  
  Long64_t nbytes = 0, nb = 0;

  std::map<long /* run */, double /* sum or sum of squares */> pressure_sum, pressure_sum2, humidity_sum, humidity_sum2;
  std::map<long /* run */, int> pressure_points, humidity_points;
  
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);    

    if((long)RUNNUMBER>130094) {

      if (VALUE > 100.0){

	// pressure value [mbar]
	if(pressure_points.find((long)RUNNUMBER) == pressure_points.end()) {
	  pressure_points[(long)RUNNUMBER]=0;
	  pressure_sum[(long)RUNNUMBER]=0.0;
	  pressure_sum2[(long)RUNNUMBER]=0.0;
	}
	pressure_points[(long)RUNNUMBER]++;
	pressure_sum[(long)RUNNUMBER]+=VALUE;
	pressure_sum2[(long)RUNNUMBER]+=VALUE*VALUE;

      } else {
	// relative humidity value [0-100%]
	if(humidity_points.find((long)RUNNUMBER) == humidity_points.end()) {
	  humidity_points[(long)RUNNUMBER]=0;
	  humidity_sum[(long)RUNNUMBER]=0.0;
	  humidity_sum2[(long)RUNNUMBER]=0.0;
	}
	humidity_points[(long)RUNNUMBER]++;
	humidity_sum[(long)RUNNUMBER]+=VALUE;
	humidity_sum2[(long)RUNNUMBER]+=VALUE*VALUE;
      }
    }

    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    // if (Cut(ientry) < 0) continue;
  } // end of loop

  // PRESSURE text and root files
  TFile fpress(Form("%s.root",foutPressurePrefix),"RECREATE");
  fpress.cd();
  TGraphErrors *gpress = new TGraphErrors();
  gpress->SetName("Graph");
  gpress->SetTitle("Pressure vs run;Run;Pressure [mbar]");

  std::ofstream ofpress;
  ofpress.open(Form("%s.dat",foutPressurePrefix),std::ofstream::trunc); // overwrite existing file

  if(!ofpress.good() || !fpress.IsOpen()) {
    std::cerr << "Can't write to PRESSURE output file" << std::endl; 
  } else {
    // calculate average and uncertainty
    int ipoint=0;
    for(std::map<long,int>::const_iterator it=pressure_points.begin();it!=pressure_points.end();it++) {
      double pave = pressure_sum[it->first]/pressure_points[it->first];
      double perr = pressure_accuracy/sqrt(3.);
      if(pressure_points[it->first]>1) {
	perr = sqrt( perr*perr +
		     (pressure_sum2[it->first]/pressure_points[it->first]-pave*pave)/(pressure_points[it->first]-1) );
      }
      ofpress << Form("%-6ld  %-8.3lf  %-8.3lf", (long)it->first /* run */, pave, perr) << std::endl;
      gpress->SetPoint(ipoint, 1.0*it->first, pave);
      gpress->SetPointError(ipoint, 0.0, perr);
      ipoint++;
    }
  }
  ofpress.close();
  gpress->Write();
  fpress.Close();

  // HUMIDITY text and root files
  TFile fhum(Form("%s.root",foutHumidityPrefix),"RECREATE");
  fhum.cd();
  TGraphErrors *ghum = new TGraphErrors();
  ghum->SetName("Graph");
  ghum->SetTitle("Relative humidity vs run;Run;Relative humidity [%]");

  std::ofstream ofhum;
  ofhum.open(Form("%s.dat",foutHumidityPrefix),std::ofstream::trunc); // overwrite existing file

  if(!ofhum.good() || !fhum.IsOpen()) {
    std::cerr << "Can't write to HUMIDITY output file" << std::endl; 
  } else {
    // calculate average and uncertainty
    int ipoint=0;
    for(std::map<long,int>::const_iterator it=humidity_points.begin();it!=humidity_points.end();it++) {
      double have = humidity_sum[it->first]/humidity_points[it->first];
      double herr = humidity_accuracy/sqrt(3.);
      if(humidity_points[it->first]>1) {
	herr = sqrt( herr*herr +
		     (humidity_sum2[it->first]/humidity_points[it->first]-have*have)/(humidity_points[it->first]-1) );
      }
      ofhum << Form("%-6ld  %-8.3lf  %-8.3lf", (long)it->first /* run */, have, herr) << std::endl;
      ghum->SetPoint(ipoint, 1.0*it->first, have);
      ghum->SetPointError(ipoint, 0.0, herr);
      ipoint++;
    }
  }
  ofhum.close();
  ghum->Write();
  fhum.Close();

}

