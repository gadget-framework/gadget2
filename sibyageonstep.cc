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
  int i;
  double minlength = Stocks[0]->returnLengthGroupDiv()->minLength();
  double maxlength = minlength;
  double stockminlength, stockmaxlength;
  for (i = 0; i < Stocks.Size(); i++) {
    stockminlength = Stocks[i]->returnLengthGroupDiv()->minLength();
    stockmaxlength = Stocks[i]->returnLengthGroupDiv()->maxLength();
    if (stockminlength < minlength)
      minlength = stockminlength;
    if (maxlength < stockmaxlength)
      maxlength = stockmaxlength;
  }

  LgrpDiv = new LengthGroupDivision(minlength, maxlength, maxlength - minlength);
  aggregator = new StockAggregator(Stocks, LgrpDiv, Areas, Ages);
}

void SIByAgeOnStep::Sum(const TimeClass* const TimeInfo) {
  if (!(this->isToSum(TimeInfo)))
    return;

  handle.logMessage("Calculating index for surveyindex component", this->SIName());
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
