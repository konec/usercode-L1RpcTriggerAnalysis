#include "UserCode/L1RpcTriggerAnalysis/interface/L1RpcTriggerAnalysisEfficiencyUtilities.h"

#include <sstream>
#include <cmath>

using namespace L1RpcTriggerAnalysisEfficiencyUtilities;

std::string Key::name()
{
    std::stringstream s;
    s << "h_" << ieta << "_" << ipt;
    return s.str();
}


//  for each index return corresponding ipt code value.   
//  exception: 0 -> returns 0. instead of "no muon"
//             1 -> returns 0.1 instead of 0.
//             32 -> is outside of L1 ptscale. Returns 160.
double PtScale::xBins[]={0., 0.1, 
                         1.5, 2., 2.5, 3., 3.5, 4., 4.5, 5., 6., 7., 8., 
                         10., 12., 14., 16., 18., 20., 25., 30., 35., 40., 45., 
                         50., 60., 70., 80., 90., 100., 120., 140., 
                         160.};

float PtScale::ptValue(unsigned int ptCode)
{
  return (ptCode < n_binspt) ? xBins[ptCode] : -1;
}

unsigned int PtScale::ptCode(float ptValue)
{
  int result = 0;
  for (unsigned int i = 1; i < n_binspt; i++)
  {
     if (ptValue >= xBins[i]+1E-3) result += 1;
     else break;
  }
  return result;
}

// for each index keep corresponding lower eta bound of a tower.
//  works only for a positive (absolute) eta. Thus for index 0 there is exception. 
//
double EtaScale::tower[]={ 0., 0.07, 0.27, 0.44, 0.58, 0.72, 0.83,  //0-6 pure barrel 
                           0.93, 1.04, 1.14,                        //7-9 barrel/endcap overlap 
                           1.24, 1.36, 1.48,                        //10-12 endcap 
                           1.61, 1.73, 1.85, 1.97, 2.1};            //13-16 high eta endcap

float EtaScale::etaValue(unsigned int etaCode)
{
  return (etaCode < n_towers) ? tower[etaCode] : -1;
}

unsigned int EtaScale::etaCode(float etaValue)
{
  float abs_Value = std::fabs(etaValue);
  unsigned int result = 0;
  for (unsigned int i = 1; i < n_towers; i++)
  {
     if (abs_Value >= tower[i]) result += 1;
     else break;
  }
  return result;
}

