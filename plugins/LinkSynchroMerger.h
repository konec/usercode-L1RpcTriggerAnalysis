#ifndef LinkSynchroMerger_H
#define LinkSynchroMerger_H

#include "LinkSynchroAnalysis.h"
#include <string>

class LinkSynchroMerger : public LinkSynchroAnalysis {
public:
  LinkSynchroMerger(const edm::ParameterSet& cfg);
  virtual ~LinkSynchroMerger(){}
  virtual void beginJob();
  virtual void beginRun(const edm::Run&, const edm::EventSetup& es);
  virtual void analyze(const edm::Event&, const edm::EventSetup&){}
private:
  void preFillFromFile(const std::string & file);
  bool isInitialised;
  std::vector<std::string> theFileNames;
};
#endif
