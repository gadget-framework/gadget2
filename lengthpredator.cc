#include "lengthpredator.h"

LengthPredator::LengthPredator(const char* givenname, const IntVector& Areas,
  const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv,
  Keeper* const keeper, Formula multi) : PopPredator(givenname, Areas, OtherLgrpDiv, GivenLgrpDiv) {

  scaler.resize(Areas.Size(), 0.0);
  Multiplicative = multi;
  keeper->addString("scale");
  Multiplicative.Inform(keeper);
  keeper->clearLast();
}

void LengthPredator::Sum(const PopInfoVector& NumberInArea, int area) {

  int i, inarea = this->areaNum(area);
  for (i = 0; i < prednumber[inarea].Size(); i++)
    prednumber[inarea][i].N = 0.0;
  prednumber[inarea].Sum(&NumberInArea, *CI);
}
