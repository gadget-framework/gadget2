#include "lengthpredator.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

LengthPredator::LengthPredator(const char* givenname, const IntVector& Areas,
  Keeper* const keeper, Formula multscaler) : PopPredator(givenname, Areas) {

  keeper->addString("scale");
  multi = multscaler;
  multi.Inform(keeper);
  keeper->clearLast();
}

void LengthPredator::Sum(const PopInfoVector& NumberInArea, int area) {
  prednumber[this->areaNum(area)].Sum(&NumberInArea, *CI);
}

void LengthPredator::Reset(const TimeClass* const TimeInfo) {
  PopPredator::Reset(TimeInfo);
  if (multi < 0.0)
    handle.logMessage(LOGWARN, "Warning in lengthpredator - negative value for multiplicative");
}
