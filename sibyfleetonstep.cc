#include "sibyfleetonstep.h"
#include "stock.h"
#include "areatime.h"
#include "loglinearregression.h"
#include "stockaggregator.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

SIByFleetOnStep::SIByFleetOnStep(CommentStream& infile, const IntMatrix& areas,
  const DoubleVector& lengths, const CharPtrVector& areaindex,
  const CharPtrVector& lenindex, const TimeClass* const TimeInfo,
  const char* datafilename, int overcons, const char* name)
  : SIOnStep(infile, datafilename, areaindex, TimeInfo, areas, lenindex, name) {

  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    handle.Message("Error in surveyindex - failed to create length group");
  overconsumption = overcons;
}

SIByFleetOnStep::~SIByFleetOnStep() {
  if (aggregator != 0)
    delete aggregator;
  if (LgrpDiv != 0)
    delete LgrpDiv;
}

void SIByFleetOnStep::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {

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

  Ages.AddRows(1, maxage - minage + 1);
  for (i = 0; i < Ages.Ncol(); i++)
    Ages[0][i] = i + minage;

  aggregator = new FleetPreyAggregator(Fleets, Stocks, LgrpDiv, Areas, Ages, overconsumption);
}

void SIByFleetOnStep::Sum(const TimeClass* const TimeInfo) {
  if (!(this->IsToSum(TimeInfo)))
    return;

  handle.logMessage("Calculating index for surveyindex component", this->SIName());
  aggregator->Sum(TimeInfo);
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
