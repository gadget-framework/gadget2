#include "sibyeffortonstep.h"
#include "areatime.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

SIByEffortOnStep::SIByEffortOnStep(CommentStream& infile, const IntMatrix& areas,
  const CharPtrVector& areaindex, const CharPtrVector& fleetindex,
  const TimeClass* const TimeInfo, const char* datafilename, const char* givenname, int bio)
  : SIOnStep(infile, datafilename, areaindex, TimeInfo, areas, fleetindex, givenname, bio, SIEFFORT) {
}

SIByEffortOnStep::~SIByEffortOnStep() {
  if (aggregator != 0)
    delete aggregator;
}

void SIByEffortOnStep::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  if (Stocks.Size() == 0)
    handle.logMessage(LOGFAIL, "Error in surveyindex - failed to initialise stock data");
  if (Fleets.Size() == 0)
    handle.logMessage(LOGFAIL, "Error in surveyindex - failed to initialise fleet data");
  aggregator = new FleetEffortAggregator(Fleets, Stocks, Areas);
}

void SIByEffortOnStep::Sum(const TimeClass* const TimeInfo) {
  if (!AAT.atCurrentTime(TimeInfo))
    return;

  int a, i, j;
  timeindex = -1;
  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
      timeindex = i;
  if (timeindex == -1)
    handle.logMessage(LOGFAIL, "Error in surveyindex - invalid timestep");

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Calculating index for surveyindex component", this->getName());

  aggregator->Sum();
  for (a = 0; a < Areas.Nrow(); a++) {
    dptr = aggregator->getSum()[a];
    for (i = 0; i < dptr->Nrow(); i++)
      for (j = 0; j < dptr->Ncol(i); j++)
        (*modelIndex[timeindex])[a][i] += (*dptr)[i][j];
  }
}
