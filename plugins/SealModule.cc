#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_SEAL_MODULE();

#include "FilterBX.h"
DEFINE_ANOTHER_FWK_MODULE(FilterBX);

#include "FilterOrbit.h"
DEFINE_ANOTHER_FWK_MODULE(FilterOrbit);

#include "Filter_L1_GM.h"
DEFINE_ANOTHER_FWK_MODULE(Filter_L1_GM);

#include "LinkSynchroAnalysis.h"
DEFINE_ANOTHER_FWK_MODULE(LinkSynchroAnalysis);

#include "LinkSynchroMerger.h"
DEFINE_ANOTHER_FWK_MODULE(LinkSynchroMerger);


