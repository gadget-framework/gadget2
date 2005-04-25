#include "sibylengthonstep.h"
#include "stock.h"
#include "areatime.h"
#include "loglinearregression.h"
#include "errorhandler.h"
#include "mathfunc.h"
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
  int i, found;
  int minage = 100;
  int maxage = 0;
  for (i = 0; i < Stocks.Size(); i++) {
    minage = min(Stocks[i]->minAge(), minage);
    maxage = max(Stocks[i]->maxAge(), maxage);
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

  aggregator = new StockAggregator(Stocks, LgrpDiv, Areas, Ages);
}

void SIByLengthOnStep::Sum(const TimeClass* const TimeInfo) {
  if (!(this->isToSum(TimeInfo)))
    return;

  handle.logMessage("Calculating index for surveyindex component", this->getSIName());
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
