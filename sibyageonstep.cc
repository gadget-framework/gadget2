#include "sibyageonstep.h"
#include "stock.h"
#include "areatime.h"
#include "loglinearregression.h"
#include "stockaggregator.h"
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

void SIByAgeOnStep::setStocks(const StockPtrVector& Stocks) {

  /* This function initialises aggregator. It should:
   *  merge all the areas in Areas
   *  merge all length groups of the stocks
   *  merge all the stocks in Stocks
   *  merge all the age groups in ages[i]
   * This means that aggregator.returnSum will return an
   * AgeBandMatrixPtrVector with only one element. That element will have
   * ages.Nrow() lines (i.e. ages) and only 1 column (i.e. 1 length group)*/

  //create a LengthGroupDivision for aggregator to use.
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
  if (!(this->IsToSum(TimeInfo)))
    return;

  handle.logMessage("Calculating index for surveyindex component", this->SIName());
  aggregator->Sum();
  //Use that the AgeBandMatrixPtrVector aggregator->returnSum returns has only one element.
  //Copy the information from it -- we only want to keep the abundance numbers.
  const AgeBandMatrix* Alptr = &(aggregator->returnSum()[0]);
  int maxage = Alptr->maxAge();
  DoubleVector numbers(maxage + 1);
  int age;
  for (age = 0; age <= maxage; age++)
    numbers[age] = (*Alptr)[age][0].N;
  this->keepNumbers(numbers);
}
