#include "sibyageonstep.h"
#include "stock.h"
#include "areatime.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

SIByAgeOnStep::SIByAgeOnStep(CommentStream& infile, const IntMatrix& areas,
  const IntMatrix& ages, const CharPtrVector& areaindex, const CharPtrVector& ageindex,
  const TimeClass* const TimeInfo, const char* datafilename, const char* givenname, int bio)
  : SIOnStep(infile, datafilename, areaindex, TimeInfo, areas, ageindex, givenname, bio, SIAGE) {

  Ages = ages;
}

SIByAgeOnStep::~SIByAgeOnStep() {
  if (aggregator != 0)
    delete aggregator;
}

void SIByAgeOnStep::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, found, minage, maxage;
  double minlength, maxlength;

  if (Stocks.Size() == 0)
    handle.logMessage(LOGFAIL, "Error in surveyindex - failed to initialise stock data");

  minlength = Stocks[0]->getLengthGroupDiv()->minLength();
  maxlength = Stocks[0]->getLengthGroupDiv()->maxLength();
  if (Stocks.Size() > 1) {
    for (i = 1; i < Stocks.Size(); i++) {
      minlength = min(Stocks[i]->getLengthGroupDiv()->minLength(), minlength);
      maxlength = max(Stocks[i]->getLengthGroupDiv()->maxLength(), maxlength);
    }
  }

  LgrpDiv = new LengthGroupDivision(minlength, maxlength, maxlength - minlength);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in surveyindex - failed to create length group");

  //check stock ages
  if (handle.getLogLevel() >= LOGWARN) {
    minage = 9999;
    maxage = 0;
    for (i = 0; i < Ages.Nrow(); i++) {
      for (j = 0; j < Ages.Ncol(i); j++) {
        minage = min(Ages[i][j], minage);
        maxage = max(Ages[i][j], maxage);
      }
    }

    found = 0;
    for (i = 0; i < Stocks.Size(); i++)
      if (minage >= Stocks[i]->minAge())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in surveyindex - minimum age less than stock age");

    found = 0;
    for (i = 0; i < Stocks.Size(); i++)
      if (maxage <= Stocks[i]->maxAge())
        found++;
    if (found == 0)
      handle.logMessage(LOGWARN, "Warning in surveyindex - maximum age greater than stock age");
  }

  aggregator = new StockAggregator(Stocks, LgrpDiv, Areas, Ages);
}

void SIByAgeOnStep::Sum(const TimeClass* const TimeInfo) {
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
  alptr = &aggregator->getSum();
  //alptr will only have one length group
  for (a = 0; a < Areas.Nrow(); a++)
    for (i = 0; i < Ages.Nrow(); i++)
      (*modelIndex[timeindex])[a][i] = (*alptr)[a][i][0].N;

  if (biomass)  //JMB experimental survey index based on the biomass
    for (a = 0; a < Areas.Nrow(); a++)
      for (i = 0; i < Ages.Nrow(); i++)
        (*modelIndex[timeindex])[a][i] *= (*alptr)[a][i][0].W;
}
