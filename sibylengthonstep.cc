#include "sibylengthonstep.h"
#include "conversion.h"
#include "stock.h"
#include "areatime.h"
#include "loglinearregression.h"
#include "stockaggregator.h"
#include "gadget.h"

SIByLengthOnStep::SIByLengthOnStep(CommentStream& infile, const intvector& areas,
  const doublevector& lengths, const char* arealabel, const charptrvector& lenindex,
  const TimeClass* const TimeInfo, const char* datafilename)
  : SIOnStep(infile, datafilename, arealabel, TimeInfo, lenindex),
    aggregator(0), LgrpDiv(0), Areas(areas) {

  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    LengthGroupPrintError(lengths, "survey indices by length");
}

SIByLengthOnStep::~SIByLengthOnStep() {
  delete aggregator;
  delete LgrpDiv;
}

void SIByLengthOnStep::SetStocks(const Stockptrvector& Stocks) {

  /* This function initializes aggregator. It should:
   *  merge all the areas in Areas
   *  merge all age groups of the stocks
   *  merge all the stocks in Stocks
   * let the length group division be according to LgrpDiv.
   * This means that aggregator.ReturnSum will return an
   * agebandmatrixptrvector with only one element. That element will have
   * 1 line (i.e. 1 age) and LgrpDiv.NoLengthGroups() columns. */

  minage = 100;
  maxage = 0;
  int i;
  for (i = 0; i < Stocks.Size(); i++) {
    if (Stocks[i]->Minage() < minage)
      minage = Stocks[i]->Minage();
    if (maxage < Stocks[i]->Maxage())
      maxage = Stocks[i]->Maxage();
  }

  intmatrix agematrix(1, maxage - minage + 1);
  for (i = 0; i < agematrix.Ncol(); i++)
    agematrix[0][i] = i + minage;
  intmatrix areamatrix(1, Areas.Size());
  for (i = 0; i < areamatrix.Ncol(); i++)
    areamatrix[0][i] = Areas[i];
  aggregator = new StockAggregator(Stocks, LgrpDiv, areamatrix, agematrix);
}

void SIByLengthOnStep::Sum(const TimeClass* const TimeInfo) {
  if (!(this->IsToSum(TimeInfo)))
    return;
  aggregator->Sum();
  //Use that the agebandmatrixptrvector aggregator->ReturnSum returns has only one element.
  //Copy the information from it -- we only want to keep the abundance numbers.
  const Agebandmatrix* Alptr = &(aggregator->ReturnSum()[0]);
  int length = Alptr->Maxlength(0);
  doublevector numbers(length);
  int len;
  for (len = 0; len < length; len++)
    numbers[len] = (*Alptr)[0][len].N;
  this->KeepNumbers(numbers);
}
