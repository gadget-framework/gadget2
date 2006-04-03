#include "sibyfleetonstep.h"
#include "stock.h"
#include "areatime.h"
#include "loglinearregression.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

SIByFleetOnStep::SIByFleetOnStep(CommentStream& infile, const IntMatrix& areas,
  const DoubleVector& lengths, const CharPtrVector& areaindex,
  const CharPtrVector& lenindex, const TimeClass* const TimeInfo,
  const char* datafilename, const char* name, int bio)
  : SIOnStep(infile, datafilename, areaindex, TimeInfo, areas, lenindex, name, bio) {

  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in surveyindex - failed to create length group");
}

SIByFleetOnStep::~SIByFleetOnStep() {
  if (aggregator != 0)
    delete aggregator;
  if (LgrpDiv != 0)
    delete LgrpDiv;
}

void SIByFleetOnStep::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, found;
  int minage = 100;
  int maxage = 0;
  for (i = 0; i < Stocks.Size(); i++) {
    minage = min(Stocks[i]->minAge(), minage);
    maxage = max(Stocks[i]->maxAge(), maxage);
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

  int i;
  timeindex = -1;
  for (i = 0; i < Years.Size(); i++)
    if ((Years[i] == TimeInfo->getYear()) && (Steps[i] == TimeInfo->getStep()))
      timeindex = i;
  if (timeindex == -1)
    handle.logMessage(LOGFAIL, "Error in surveyindex - invalid timestep");

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Calculating index for surveyindex component", this->getSIName());
  aggregator->Sum();
  if ((handle.getLogLevel() >= LOGWARN) && (aggregator->checkCatchData() == 1))
    handle.logMessage(LOGWARN, "Warning in surveyindex - zero catch found");
  alptr = &(aggregator->getSum()[0]);
  //JMB experimental survey index based on the biomass
  if (biomass == 1)
    for (i = 0; i < this->numIndex(); i++)
      modelIndex[timeindex][i] = (*alptr)[0][i].N * (*alptr)[0][i].W;
  else
    for (i = 0; i < this->numIndex(); i++)
      modelIndex[timeindex][i] = (*alptr)[0][i].N;
}
