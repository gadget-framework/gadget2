#include "sibyacousticonstep.h"
#include "stock.h"
#include "areatime.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "gadget.h"
#include "global.h"

SIByAcousticOnStep::SIByAcousticOnStep(CommentStream& infile, const IntMatrix& areas,
  const CharPtrVector& areaindex, const CharPtrVector& fleetindex,
  const TimeClass* const TimeInfo, const char* datafilename, const char* givenname, int bio)
  : SIOnStep(infile, datafilename, areaindex, TimeInfo, areas, fleetindex, givenname, bio, SIACOUSTIC) {
}

SIByAcousticOnStep::~SIByAcousticOnStep() {
  if (aggregator != 0)
    delete aggregator;
}

void SIByAcousticOnStep::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, minage, maxage;
  double minlength, maxlength;

  if (Stocks.Size() == 0)
    handle.logMessage(LOGFAIL, "Error in surveyindex - failed to initialise stock data");

  minage = Stocks[0]->minAge();
  maxage = Stocks[0]->maxAge();
  minlength = Stocks[0]->getLengthGroupDiv()->minLength();
  maxlength = Stocks[0]->getLengthGroupDiv()->maxLength();

  if (Stocks.Size() > 1) {
    for (i = 1; i < Stocks.Size(); i++) {
      minlength = min(Stocks[i]->getLengthGroupDiv()->minLength(), minlength);
      maxlength = max(Stocks[i]->getLengthGroupDiv()->maxLength(), maxlength);
      minage = min(Stocks[i]->minAge(), minage);
      maxage = max(Stocks[i]->maxAge(), maxage);
    }
  }

  LgrpDiv = new LengthGroupDivision(minlength, maxlength, maxlength - minlength);
  if (LgrpDiv->Error())
    handle.logMessage(LOGFAIL, "Error in surveyindex - failed to create length group");

  Ages.AddRows(1, maxage - minage + 1, 0);
  for (i = 0; i < Ages.Ncol(); i++)
    Ages[0][i] = i + minage;

  aggregator = new StockAggregator(Stocks, LgrpDiv, Areas, Ages);
}

void SIByAcousticOnStep::Sum(const TimeClass* const TimeInfo) {
  if (!AAT.atCurrentTime(TimeInfo))
    return;

  int a;
  timeindex = -1;
  for (a = 0; a < Years.Size(); a++)
    if ((Years[a] == TimeInfo->getYear()) && (Steps[a] == TimeInfo->getStep()))
      timeindex = a;
  if (timeindex == -1)
    handle.logMessage(LOGFAIL, "Error in surveyindex - invalid timestep");

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Calculating index for surveyindex component", this->getName());

  aggregator->Sum();
  alptr = &aggregator->getSum();
  //alptr will only have one length group and one age group
  for (a = 0; a < Areas.Nrow(); a++)
    (*modelIndex[timeindex])[a][0] = (*alptr)[a][0][0].N;

  if (biomass)
    for (a = 0; a < Areas.Nrow(); a++)
      (*modelIndex[timeindex])[a][0] *= (*alptr)[a][0][0].W;
}
