#include "catch.h"
#include "conversion.h"
#include "stock.h"

//warning: incomplete.

Catch::Catch(CatchData* CD) : CI(0) {
  Catchdata = CD;
}

void Catch::SetCI(const LengthGroupDivision* const LgrpDiv) {
  CI = new ConversionIndex(Catchdata->ReturnLengthGroupDiv(), LgrpDiv);
}

Catch::~Catch() {
  delete CI;
}

//Reduce the population of the stock by the catch.
void Catch::Subtract(Agebandmatrix& Alkeys, int area, const TimeClass* const TimeInfo) {
  if (this->GetCatch(area, TimeInfo) != 0)
    AgebandmSubtract(Alkeys, *(this->GetCatch(area, TimeInfo)), *CI, 0);
}

void Catch::Subtract(Stock& stock, int area) {
  const bandmatrix* c  = Catchdata->GetCatch(stock);
  if (c != 0)
    #ifndef ALLOW_NEGATIVE_STOCK
      AgebandmSubtract(stock.MutableAgelengthkeys(area), *c, *CI, 0);
    #else
      AgebandmSubtract(stock.MutableAgelengthkeys(area), *c, *CI, 1);
    #endif
}

const bandmatrix* Catch::GetCatch(int area, const TimeClass* const TimeInfo) const {
  return Catchdata->GetCatch(area, TimeInfo);
}
