#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FilterBX.h"
DEFINE_FWK_MODULE(FilterBX);

#include "FilterOrbit.h"
DEFINE_FWK_MODULE(FilterOrbit);

#include "UserCode/L1RpcTriggerAnalysis/interface/FilterMenu.h"
DEFINE_FWK_MODULE(FilterMenu);

#include "FilterL1.h"
DEFINE_FWK_MODULE(FilterL1);

#include "FilterGM.h"
DEFINE_FWK_MODULE(FilterGM);

#include "Filter_L1_GM.h"
DEFINE_FWK_MODULE(Filter_L1_GM);

#include "L1RpcTreeMaker.h"
DEFINE_FWK_MODULE(L1RpcTreeMaker);

#include "L1RpcTreeAnalysis.h"
DEFINE_FWK_MODULE(L1RpcTreeAnalysis);

#include "L1RpcMCEfficiency.h"
DEFINE_FWK_MODULE(L1RpcMCEfficiency);

#include "OMTFROOTReader.h"
DEFINE_FWK_MODULE(OMTFROOTReader);

#include "OMTFAnalyzer.h"
DEFINE_FWK_MODULE(OMTFAnalyzer);

