#include "sibylengthonstep.h"
#include "stock.h"
#include "areatime.h"
#include "loglinearregression.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

SIByLengthOnStep::SIByLengthOnStep(CommentStream& infile, const IntMatrix& areas,
  const DoubleVector& lengths, const CharPtrVector& areaindex,
  const CharPtrVector& lenindex, const TimeClass* const TimeInfo,
  const char* datafilename, const char* name)
  : SIOnStep(infile, datafilename, areaindex, TimeInfo, areas, lenindex, name) {

  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    handle.Message("Error in surveyindex - failed to create length group");
}

SIByLengthOnStep::~SIByLengthOnStep() {
  if (aggregator != 0)
    delete aggregator;
  if (LgrpDiv != 0)
    delete LgrpDiv;
}

void SIByLengthOnStep::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i;
  int minage = 100;
  int maxage = 0;
  for (i = 0; i < Stocks.Size(); i++) {
    if (Stocks[i]->minAge() < minage)
      minage = Stocks[i]->minAge();
    if (maxage < Stocks[i]->maxAge())
      maxage = Stocks[i]->maxAge();
  }

  Ages.AddRows(1, maxage - minage + 1);
  for (i = 0; i < Ages.Ncol(); i++)
    Ages[0][i] = i + minage;

  aggregator = new StockAggregator(Stocks, LgrpDiv, Areas, Ages);
}

void SIByLengthOnStep::Sum(const TimeClass* const TimeInfo) {
  if (!(this->isToSum(TimeInfo)))
    return;

  handle.logMessage("Calculating index for surveyindex component", this->SIName());
  aggregator->Sum();
  //Use that the AgeBandMatrixPtrVector aggregator->returnSum returns has only one element.
  //Copy the information from it -- we only want to keep the abundance numbers.
  const AgeBandMatrix* Alptr = &(aggregator->returnSum()[0]);
  DoubleVector numbers(Alptr->maxLength(0), 0.0);
  int len;
  for (len = 0; len < numbers.Size(); len++)
    numbers[len] = (*Alptr)[0][len].N;
  this->keepNumbers(numbers);
}
