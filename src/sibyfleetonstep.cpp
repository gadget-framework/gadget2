#include "sibyfleetonstep.h"
#include "stock.h"
#include "areatime.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

SIByFleetOnStep::SIByFleetOnStep(CommentStream& infile, const IntMatrix& areas,
  const DoubleVector& lengths, const CharPtrVector& areaindex,
  const CharPtrVector& lenindex, const TimeClass* const TimeInfo,
  const char* datafilename, const char* givenname, int bio)
  : SIOnStep(infile, datafilename, areaindex, TimeInfo, areas, lenindex, givenname, bio, SIFLEET) {

  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in surveyindex - failed to create length group");
}

SIByFleetOnStep::~SIByFleetOnStep() {
  if (aggregator != 0)
    delete aggregator;
}

void SIByFleetOnStep::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, found, minage, maxage;

  if (Stocks.Size() == 0)
    handle.logMessage(LOGFAIL, "Error in surveyindex - failed to initialise stock data");
  if (Fleets.Size() == 0)
    handle.logMessage(LOGFAIL, "Error in surveyindex - failed to initialise fleet data");

  minage = Stocks[0]->minAge();
  maxage = Stocks[0]->maxAge();
  if (Stocks.Size() > 1) {
    for (i = 1; i < Stocks.Size(); i++) {
      minage = min(Stocks[i]->minAge(), minage);
      maxage = max(Stocks[i]->maxAge(), maxage);
    }
  }

  Ages.AddRows(1, maxage - minage + 1, 0);
  for (i = 0; i < Ages.Ncol(); i++)
    Ages[0][i] = i + minage;

  //check stock lengths
  if (handle.getLogLevel() >= LOGWARN) {
    found = 0;
    for (i = 0; i < Stocks.Size(); i++)
      if (LgrpDiv->maxLength(0) > Stocks[i]->getLengthGroupDiv()->minLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in surveyindex - minimum length group less than stock length");

    found = 0;
    for (i = 0; i < Stocks.Size(); i++)
      if (LgrpDiv->minLength(LgrpDiv->numLengthGroups()) < Stocks[i]->getLengthGroupDiv()->maxLength())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in surveyindex - maximum length group greater than stock length");
  }

  aggregator = new FleetPreyAggregator(Fleets, Stocks, LgrpDiv, Areas, Ages, 0);
}

void SIByFleetOnStep::Sum(const TimeClass* const TimeInfo) {
  if (!AAT.atCurrentTime(TimeInfo))
    return;

  int a, i;
  timeindex = -1;
  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
      timeindex = i;
  if (timeindex == -1)
    handle.logMessage(LOGFAIL, "Error in surveyindex - invalid timestep");

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Calculating index for surveyindex component", this->getName());

  aggregator->Sum();
  if ((handle.getLogLevel() >= LOGWARN) && (aggregator->checkCatchData()))
    handle.logMessage(LOGWARN, "Warning in surveyindex - zero catch found");

  alptr = &aggregator->getSum();
  //alptr will only have one age group
  for (a = 0; a < Areas.Nrow(); a++)
    for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
      (*modelIndex[timeindex])[a][i] = (*alptr)[a][0][i].N;

  if (biomass)  //JMB experimental survey index based on the biomass
    for (a = 0; a < Areas.Nrow(); a++)
      for (i = 0; i < LgrpDiv->numLengthGroups(); i++)
        (*modelIndex[timeindex])[a][i] *= (*alptr)[a][0][i].W;
}
