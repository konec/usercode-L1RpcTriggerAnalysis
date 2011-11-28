#include "UserCode/L1RpcTriggerAnalysis/interface/Utilities.h"

#include <sstream>
#include <cmath>

//  for each index return corresponding ipt code value.   
//  exception: 0 -> returns 0. instead of "no muon"
//             1 -> returns 0.1 instead of 0.
//             32 -> is outside of L1 ptscale. Returns 160.
double L1PtScale::ptBins[]={0., 0.1, 
                         1.5, 2., 2.5, 3., 3.5, 4., 4.5, 5., 6., 7., 8., 
                         10., 12., 14., 16., 18., 20., 25., 30., 35., 40., 45., 
                         50., 60., 70., 80., 90., 100., 120., 140., 
                         160. };

double L1RpcEtaScale::etaBins[] = {
   -2.10, -1.97, -1.85, -1.73, -1.61, -1.48, -1.36, -1.24, -1.14,
   -1.04, -0.93, -0.83, -0.72, -0.58, -0.44, -0.27, -0.07,
    0.07, 0.27, 0.44, 0.58, 0.72, 0.83, 0.93, 1.04, 1.14,
    1.24, 1.36, 1.48, 1.61, 1.73, 1.85, 1.97, 2.10 };


float L1PtScale::ptValue(unsigned int ptCode)
{
  return (ptCode < nPtBins) ? ptBins[ptCode] : -1;
}

unsigned int L1PtScale::ptCode(float ptValue)
{
  unsigned int result = 0;
  for (unsigned int i = 0; i < nPtBins; i++) {
    if (std::fabs(ptValue) >= ptBins[i]) result = i; else break;
  }
  return result;
}

// for each index keep corresponding lower eta bound of a tower.
float L1RpcEtaScale::etaValue(int etaCode)
{
  if (etaCode > 16) return 2.4;
  else if (etaCode < -16) return -2.4;

  unsigned int etaBin = 16+etaCode;
  return (etaBins[etaBin]+etaBins[etaBin+1])/2.;
}

int L1RpcEtaScale::etaCode(float etaValue)
{
  int result = -17;
  for (unsigned int i = 0; i <= nEtaBins; i++) {
    if (etaValue >= etaBins[i]) result += 1; else break;
  }
  return result;
}

