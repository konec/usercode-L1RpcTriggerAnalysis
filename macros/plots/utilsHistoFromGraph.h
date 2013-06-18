#ifndef utilsHistoFromGraph_C
#define utilsHistoFromGraph_C

#include "TH1D.h"
#include <utility>

class TGraph;
class TGaxis;


TH1D runHistoFromGraph(const TGraph* gr);

void fillHistoFromGraph(TH1D &histo, TGraph* graph);

//std::pair<TH1D,TGaxis* > getDatOnRef(const TH1D &histoDat, const TH1D &histoRef);

TH1D *getPressVsRunHisto(TH1D *histo);

TH1D *getHumidityVsRunHisto(TH1D *histo);

TH1D *getClusterSizeVsRunHisto(TH1D *histo, const char *fname);

void printRunsOnPlotLinScale (TH1D* histo);
void printRunsOnPlot(TH1D* histo);

#endif

