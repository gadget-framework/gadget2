#ifndef stockpreystdinfobylength_h
#define stockpreystdinfobylength_h

#include "abstrpreystdinfobylength.h"

class StockPreyStdInfoByLength;
class StockPrey;
class LengthGroupDivision;

class StockPreyStdInfoByLength : public AbstrPreyStdInfoByLength {
public:
  StockPreyStdInfoByLength(const StockPrey* prey, const IntVector& areas);
  virtual ~StockPreyStdInfoByLength();
  virtual void Sum(const TimeClass* const TimeInfo, int area);
  const LengthGroupDivision* ReturnPreyLengthGroupDiv() const { return PreyLgrpDiv; };
private:
  const StockPrey* prey;
  const LengthGroupDivision* PreyLgrpDiv;
};

#endif
