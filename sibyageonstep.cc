#include "sibyageonstep.h"
#include "stock.h"
#include "areatime.h"
#include "loglinearregression.h"
#include "mathfunc.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

SIByAgeOnStep::SIByAgeOnStep(CommentStream& infile, const IntMatrix& areas,
  const IntMatrix& ages, const CharPtrVector& areaindex, const CharPtrVector& ageindex,
  const TimeClass* const TimeInfo, const char* datafilename, const char* name)
  : SIOnStep(infile, datafilename, areaindex, TimeInfo, areas, ageindex, name), Ages(ages) {

}

SIByAgeOnStep::~SIByAgeOnStep() {
  if (aggregator != 0)
    delete aggregator;
  if (LgrpDiv != 0)
    delete LgrpDiv;
}

void SIByAgeOnStep::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, j, found, minage, maxage;
  double minlength = Stocks[0]->returnLengthGroupDiv()->minLength();
  double maxlength = minlength;

  for (i = 0; i < Stocks.Size(); i++) {
    minlength = min(Stocks[i]->returnLengthGroupDiv()->minLength(), minlength);
    maxlength = max(Stocks[i]->returnLengthGroupDiv()->maxLength(), maxlength);
  }
  LgrpDiv = new LengthGroupDivision(minlength, maxlength, maxlength - minlength);

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
  if (!(this->isToSum(TimeInfo)))
    return;

  if (handle.getLogLevel() >= LOGMESSAGE)
    handle.logMessage(LOGMESSAGE, "Calculating index for surveyindex component", this->getSIName());
  aggregator->Sum();
  alptr = &(aggregator->returnSum()[0]);
  int i;
  for (i = 0; i < this->numIndex(); i++)
    modelIndex[timeindex][i] = (*alptr)[i][0].N;
  timeindex++;
}
