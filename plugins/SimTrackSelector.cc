#include "FWCore/Framework/interface/MakerMacros.h"

#include "CommonTools/UtilAlgos/interface/ObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/SingleObjectSelector.h"

#include "CommonTools/UtilAlgos/interface/SingleElementCollectionSelector.h"
#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/EtaRangeSelector.h"
#include "CommonTools/UtilAlgos/interface/ObjectCountFilter.h"

#include "SimDataFormats/Track/interface/SimTrack.h"

//typedef ObjectSelector< SingleElementCollectionSelector< std::vector<SimTrack>, StringCutObjectSelector<SimTrack> > > SimTrackSelector;

typedef SingleObjectSelector< std::vector<SimTrack>, StringCutObjectSelector<SimTrack> > SimTrackSelector;

DEFINE_FWK_MODULE(SimTrackSelector);


typedef ObjectCountFilter< std::vector<SimTrack>, StringCutObjectSelector<SimTrack> >::type SimTrackEtaFilter;

//typedef ObjectCountFilter<std::vector<SimTrack>, SimTrackSelector>::type SimTrackEtaFilter;
 
DEFINE_FWK_MODULE(SimTrackEtaFilter);
