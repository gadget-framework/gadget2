#ifndef stockpreystdinfo_h
#define stockpreystdinfo_h

#include "abstrpreystdinfo.h"
#include "stockpreystdinfobylength.h"

class StockPreyStdInfo;
class StockPrey;
class TimeClass;
class LengthGroupDivision;

class StockPreyStdInfo : public AbstrPreyStdInfo {
public:
  StockPreyStdInfo(const StockPrey* p, const IntVector& Areas);
  virtual ~StockPreyStdInfo();
  virtual const DoubleVector& NconsumptionByLength(int area) const;
  virtual const DoubleVector& BconsumptionByLength(int area) const;
  virtual const DoubleVector& MortalityByLength(int area) const;
  virtual void Sum(const TimeClass* const TimeInfo, int area);
  const LengthGroupDivision* ReturnPreyLengthGroupDiv() const;
private:
  StockPreyStdInfoByLength SPByLength;
  const StockPrey* prey;
};

#endif
