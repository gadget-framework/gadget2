#include "sibyfleetonstep.h"
#include "stock.h"
#include "areatime.h"
#include "loglinearregression.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

SIByFleetOnStep::SIByFleetOnStep(CommentStream& infile, const IntMatrix& areas,
  const DoubleVector& lengths, const CharPtrVector& areaindex,
  const CharPtrVector& lenindex, const TimeClass* const TimeInfo,
  const char* datafilename, const char* name)
  : SIOnStep(infile, datafilename, areaindex, TimeInfo, areas, lenindex, name) {

  LgrpDiv = new LengthGroupDivision(lengths);
  if (LgrpDiv->Error())
    handle.Message("Error in surveyindex - failed to create length group");
}

SIByFleetOnStep::~SIByFleetOnStep() {
  if (aggregator != 0)
    delete aggregator;
  if (LgrpDiv != 0)
    delete LgrpDiv;
}

void SIByFleetOnStep::setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {
  int i, found;
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

  //check stock lengths
  found = 0;
  for (i = 0; i < Stocks.Size(); i++)
    if (LgrpDiv->maxLength(0) > Stocks[i]->returnLengthGroupDiv()->minLength())
      found++;
  if (found == 0)
    handle.logWarning("Warning in surveyindex - minimum length group less than stock length");

  found = 0;
  for (i = 0; i < Stocks.Size(); i++)
    if (LgrpDiv->minLength(LgrpDiv->numLengthGroups()) < Stocks[i]->returnLengthGroupDiv()->maxLength())
      found++;
  if (found == 0)
    handle.logWarning("Warning in surveyindex - maximum length group greater than stock length");

  aggregator = new FleetPreyAggregator(Fleets, Stocks, LgrpDiv, Areas, Ages, 0);
}

void SIByFleetOnStep::Sum(const TimeClass* const TimeInfo) {
  if (!(this->isToSum(TimeInfo)))
    return;

  handle.logMessage("Calculating index for surveyindex component", this->getSIName());
  aggregator->Sum(TimeInfo);
  if (aggregator->checkCatchData() == 1)
    handle.logWarning("Warning in surveyindex - zero catch found");
  //Use that the AgeBandMatrixPtrVector aggregator->returnSum returns has only one element.
  //Copy the information from it -- we only want to keep the abundance numbers.
  const AgeBandMatrix* Alptr = &(aggregator->returnSum()[0]);
  DoubleVector numbers(Alptr->maxLength(0), 0.0);
  int len;
  for (len = 0; len < numbers.Size(); len++)
    numbers[len] = (*Alptr)[0][len].N;
  this->keepNumbers(numbers);
}
