#include "sibyageonstep.h"
#include "stock.h"
#include "areatime.h"
#include "loglinearregression.h"
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
    if (Stocks[i]->returnLengthGroupDiv()->minLength() < minlength)
      minlength = Stocks[i]->returnLengthGroupDiv()->minLength();
    if (maxlength < Stocks[i]->returnLengthGroupDiv()->maxLength())
      maxlength = Stocks[i]->returnLengthGroupDiv()->maxLength();
  }
  LgrpDiv = new LengthGroupDivision(minlength, maxlength, maxlength - minlength);

  //check stock ages
  minage = 9999;
  maxage = -1;
  for (i = 0; i < Ages.Nrow(); i++) {
    for (j = 0; j < Ages.Ncol(i); j++) {
      if (Ages[i][j] < minage)
        minage = Ages[i][j];
      if (maxage < Ages[i][j])
        maxage = Ages[i][j];
    }
  }

  found = 0;
  for (i = 0; i < Stocks.Size(); i++)
    if (minage >= Stocks[i]->minAge())
      found++;
  if (found == 0)
    handle.logWarning("Warning in surveyindex - minimum age less than stock age");

  found = 0;
  for (i = 0; i < Stocks.Size(); i++)
    if (maxage <= Stocks[i]->maxAge())
      found++;
  if (found == 0)
    handle.logWarning("Warning in surveyindex - maximum age greater than stock age");

  aggregator = new StockAggregator(Stocks, LgrpDiv, Areas, Ages);
}

void SIByAgeOnStep::Sum(const TimeClass* const TimeInfo) {
  if (!(this->isToSum(TimeInfo)))
    return;

  handle.logMessage("Calculating index for surveyindex component", this->getSIName());
  aggregator->Sum();
  //Use that the AgeBandMatrixPtrVector aggregator->returnSum returns has only one element.
  //Copy the information from it -- we only want to keep the abundance numbers.
  const AgeBandMatrix* Alptr = &(aggregator->returnSum()[0]);
  DoubleVector numbers(Alptr->maxAge() + 1, 0.0);
  int age;
  for (age = 0; age < numbers.Size(); age++)
    numbers[age] = (*Alptr)[age][0].N;
  this->keepNumbers(numbers);
}
