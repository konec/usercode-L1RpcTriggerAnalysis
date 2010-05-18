#include "LinkSynchroMerger.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <sstream>
#include <fstream>

using namespace edm;
using namespace std;



LinkSynchroMerger::LinkSynchroMerger(const edm::ParameterSet& cfg)
  : LinkSynchroAnalysis(cfg), isInitialised(false), 
   theFileNames( cfg.getUntrackedParameter<std::vector<std::string> >("preFillLinkSynchroFileNames"))
{}

void LinkSynchroMerger::beginJob()
{ RPCMonitorLinkSynchro::beginJob(); }

void LinkSynchroMerger::beginRun(const edm::Run& ev, const edm::EventSetup& es)
{
  RPCMonitorLinkSynchro::beginRun(ev,es);
  if(!isInitialised) {
    isInitialised = true;
    if (!theFileNames.empty())for (std::vector<std::string>::const_iterator it=theFileNames.begin(); it != theFileNames.end(); ++it) preFillFromFile(*it);
  }
}

void LinkSynchroMerger::preFillFromFile(const std::string & fileName)
{
  std::ifstream file( fileName.c_str() );
  if ( !file ) {
    edm::LogError(" ** LinkSynchroMerger ** ") << " cant open data file: " << fileName;
    return;
  } else {
    edm::LogInfo("LinkSynchroMerger, read data from: ") <<fileName;
  }
  string line, lbName, tmp;
  unsigned int hits[8];
  while (getline(file,line) ) {
    stringstream str(line);
    str >> lbName
        >>tmp>>tmp>>tmp>>tmp>>tmp
        >>hits[0]>>hits[1]>>hits[2]>>hits[3]>>hits[4]>>hits[5]>>hits[6]>>hits[7];
    if (str.good()) theSynchroStat.add(lbName,hits); 
  }
  file.close();
}
