#include "sibyageonstep.h"
#include "stock.h"
#include "areatime.h"
#include "loglinearregression.h"
#include "stockaggregator.h"
#include "gadget.h"

SIByAgeOnStep::SIByAgeOnStep(CommentStream& infile, const IntMatrix& areas,
  const IntMatrix& ages, const CharPtrVector& areaindex, const CharPtrVector& ageindex,
  const TimeClass* const TimeInfo, const char* datafilename, const char* name)
  : SIOnStep(infile, datafilename, areaindex, TimeInfo, areas, ageindex, name),
    aggregator(0), Ages(ages) {
}

SIByAgeOnStep::~SIByAgeOnStep() {
  delete aggregator;
}

void SIByAgeOnStep::SetStocks(const StockPtrVector& Stocks) {

  /* This function initializes aggregator. It should:
   *  merge all the areas in Areas
   *  merge all length groups of the stocks
   *  merge all the stocks in Stocks
   *  merge all the age groups in Ages[i]
   * This means that aggregator.ReturnSum will return an
   * AgeBandMatrixPtrVector with only one element. That element will have
   * Ages.Nrow() lines (i.e. ages) and only 1 column (i.e. 1 length group)*/

  //create a LengthGroupDivision for aggregator to use.
  int i;
  double minlength = Stocks[0]->ReturnLengthGroupDiv()->minLength();
  double maxlength = minlength;
  double stockminlength, stockmaxlength;
  for (i = 0; i < Stocks.Size(); i++) {
    stockminlength = Stocks[i]->ReturnLengthGroupDiv()->minLength();
    stockmaxlength = Stocks[i]->ReturnLengthGroupDiv()->maxLength();
    if (stockminlength < minlength)
      minlength = stockminlength;
    if (maxlength < stockmaxlength)
      maxlength = stockmaxlength;
  }

  LengthGroupDivision* LgrpDiv = new LengthGroupDivision(minlength, maxlength, maxlength - minlength);
  aggregator = new StockAggregator(Stocks, LgrpDiv, Areas, Ages);
}

void SIByAgeOnStep::Sum(const TimeClass* const TimeInfo) {
  if (!(this->IsToSum(TimeInfo)))
    return;
  aggregator->Sum();
  //Use that the AgeBandMatrixPtrVector aggregator->ReturnSum returns has only one element.
  //Copy the information from it -- we only want to keep the abundance numbers.
  const AgeBandMatrix* Alptr = &(aggregator->ReturnSum()[0]);
  int maxage = Alptr->Maxage();
  DoubleVector numbers(maxage + 1);
  int age;
  for (age = 0; age <= maxage; age++)
    numbers[age] = (*Alptr)[age][0].N;
  this->KeepNumbers(numbers);
}
