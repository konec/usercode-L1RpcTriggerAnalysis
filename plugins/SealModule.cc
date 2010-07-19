#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FilterBX.h"
DEFINE_FWK_MODULE(FilterBX);

#include "FilterOrbit.h"
DEFINE_FWK_MODULE(FilterOrbit);

#include "FilterL1.h"
DEFINE_FWK_MODULE(FilterL1);

#include "Filter_L1_GM.h"
DEFINE_FWK_MODULE(Filter_L1_GM);

#include "LinkSynchroAnalysis.h"
DEFINE_FWK_MODULE(LinkSynchroAnalysis);

#include "LinkSynchroMerger.h"
DEFINE_FWK_MODULE(LinkSynchroMerger);

#include "EfficiencyTree.h"
DEFINE_FWK_MODULE(EfficiencyTree);

#include "EfficiencyAnalysis.h"
DEFINE_FWK_MODULE(EfficiencyAnalysis);
