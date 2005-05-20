#include "lengthpredator.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

LengthPredator::LengthPredator(const char* givenname, const IntVector& Areas,
  Keeper* const keeper, Formula multscaler) : PopPredator(givenname, Areas) {

  scaler.resize(Areas.Size(), 0.0);
  multi = multscaler;
  keeper->addString("scale");
  multi.Inform(keeper);
  keeper->clearLast();
}

void LengthPredator::Sum(const PopInfoVector& NumberInArea, int area) {
  int i, inarea = this->areaNum(area);
  for (i = 0; i < prednumber[inarea].Size(); i++)
    prednumber[inarea][i].N = 0.0;
  prednumber[inarea].Sum(&NumberInArea, *CI);
}

void LengthPredator::Reset(const TimeClass* const TimeInfo) {
  PopPredator::Reset(TimeInfo);
  if ((handle.getLogLevel() >= LOGWARN) && (multi < 0))
    handle.logMessage(LOGWARN, "Warning in lengthpredator - negative value for multiplicative");
}
