#include "sibylengthonstep.h"
#include "stock.h"
#include "areatime.h"
#include "loglinearregression.h"
#include "stockaggregator.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

SIByLengthOnStep::SIByLengthOnStep(CommentStream& infile, const IntMatrix& areas,
  const DoubleVector& lengths, const CharPtrVector& areaindex, const CharPtrVector& lenindex,
  const TimeClass* const TimeInfo, const char* datafilename, const char* name)
  : SIOnStep(infile, datafilename, areaindex, TimeInfo, areas, lenindex, name),
    aggregator(0), LgrpDiv(0) {

  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    handle.Message("Error in surveyindex - failed to create length group");
}

SIByLengthOnStep::~SIByLengthOnStep() {
  delete aggregator;
  delete LgrpDiv;
}

void SIByLengthOnStep::setStocks(const StockPtrVector& Stocks) {

  /* This function initialises aggregator. It should:
   *  merge all the areas in Areas
   *  merge all age groups of the stocks
   *  merge all the stocks in Stocks
   * let the length group division be according to LgrpDiv.
   * This means that aggregator.returnSum will return an
   * AgeBandMatrixPtrVector with only one element. That element will have
   * 1 line (i.e. 1 age) and LgrpDiv.NoLengthGroups() columns. */

  int minage = 100;
  int maxage = 0;
  int i;
  for (i = 0; i < Stocks.Size(); i++) {
    if (Stocks[i]->minAge() < minage)
      minage = Stocks[i]->minAge();
    if (maxage < Stocks[i]->maxAge())
      maxage = Stocks[i]->maxAge();
  }

  IntMatrix agematrix(1, maxage - minage + 1);
  for (i = 0; i < agematrix.Ncol(); i++)
    agematrix[0][i] = i + minage;

  aggregator = new StockAggregator(Stocks, LgrpDiv, Areas, agematrix);
}

void SIByLengthOnStep::Sum(const TimeClass* const TimeInfo) {
  if (!(this->IsToSum(TimeInfo)))
    return;
  aggregator->Sum();
  //Use that the AgeBandMatrixPtrVector aggregator->returnSum returns has only one element.
  //Copy the information from it -- we only want to keep the abundance numbers.
  const AgeBandMatrix* Alptr = &(aggregator->returnSum()[0]);
  int length = Alptr->maxLength(0);
  DoubleVector numbers(length);
  int len;
  for (len = 0; len < length; len++)
    numbers[len] = (*Alptr)[0][len].N;
  this->keepNumbers(numbers);
}
