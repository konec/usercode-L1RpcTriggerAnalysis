#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FilterBX.h"
DEFINE_FWK_MODULE(FilterBX);

#include "FilterOrbit.h"
DEFINE_FWK_MODULE(FilterOrbit);

#include "FilterMenu.h"
DEFINE_FWK_MODULE(FilterMenu);

#include "FilterL1.h"
DEFINE_FWK_MODULE(FilterL1);

#include "FilterGM.h"
DEFINE_FWK_MODULE(FilterGM);

#include "Filter_L1_GM.h"
DEFINE_FWK_MODULE(Filter_L1_GM);

#include "LinkSynchroAnalysis.h"
DEFINE_FWK_MODULE(LinkSynchroAnalysis);

#include "LinkSynchroMerger.h"
DEFINE_FWK_MODULE(LinkSynchroMerger);

#include "L1RpcEfficiencyTreeMaker.h"
DEFINE_FWK_MODULE(L1RpcEfficiencyTreeMaker);

#include "L1RpcEfficiencyTreeAnalysis.h"
DEFINE_FWK_MODULE(L1RpcEfficiencyTreeAnalysis);

#include "EfficiencyTree.h"
DEFINE_FWK_MODULE(EfficiencyTree);

#include "EfficiencyAnalysis.h"
DEFINE_FWK_MODULE(EfficiencyAnalysis);

#include "L1RpcMCEfficiency.h"
DEFINE_FWK_MODULE(L1RpcMCEfficiency);

