#include "sibylengthonstep.h"
#include "stock.h"
#include "areatime.h"
#include "loglinearregression.h"
#include "stockaggregator.h"
#include "gadget.h"

SIByLengthOnStep::SIByLengthOnStep(CommentStream& infile, const IntMatrix& areas,
  const DoubleVector& lengths, const CharPtrVector& areaindex, const CharPtrVector& lenindex,
  const TimeClass* const TimeInfo, const char* datafilename, const char* name)
  : SIOnStep(infile, datafilename, areaindex, TimeInfo, areas, lenindex, name),
    aggregator(0), LgrpDiv(0) {

  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    printLengthGroupError(lengths, "survey indices by length");
}

SIByLengthOnStep::~SIByLengthOnStep() {
  delete aggregator;
  delete LgrpDiv;
}

void SIByLengthOnStep::SetStocks(const StockPtrVector& Stocks) {

  /* This function initializes aggregator. It should:
   *  merge all the areas in Areas
   *  merge all age groups of the stocks
   *  merge all the stocks in Stocks
   * let the length group division be according to LgrpDiv.
   * This means that aggregator.ReturnSum will return an
   * AgeBandMatrixPtrVector with only one element. That element will have
   * 1 line (i.e. 1 age) and LgrpDiv.NoLengthGroups() columns. */

  int minage = 100;
  int maxage = 0;
  int i;
  for (i = 0; i < Stocks.Size(); i++) {
    if (Stocks[i]->Minage() < minage)
      minage = Stocks[i]->Minage();
    if (maxage < Stocks[i]->Maxage())
      maxage = Stocks[i]->Maxage();
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
  //Use that the AgeBandMatrixPtrVector aggregator->ReturnSum returns has only one element.
  //Copy the information from it -- we only want to keep the abundance numbers.
  const AgeBandMatrix* Alptr = &(aggregator->ReturnSum()[0]);
  int length = Alptr->Maxlength(0);
  DoubleVector numbers(length);
  int len;
  for (len = 0; len < length; len++)
    numbers[len] = (*Alptr)[0][len].N;
  this->KeepNumbers(numbers);
}
