#ifndef stockpreystdinfo_h
#define stockpreystdinfo_h

#include "abstrpreystdinfo.h"
#include "stockpreystdinfobylength.h"

class StockPreyStdInfo;
class StockPrey;
class TimeClass;

class StockPreyStdInfo : public AbstrPreyStdInfo {
public:
  StockPreyStdInfo(const StockPrey* p, const intvector& Areas);
  virtual ~StockPreyStdInfo();
  virtual const doublevector& NconsumptionByLength(int area) const;
  virtual const doublevector& BconsumptionByLength(int area) const;
  virtual const doublevector& MortalityByLength(int area) const;
  virtual void Sum(const TimeClass* const TimeInfo, int area);
private:
  StockPreyStdInfoByLength SPByLength;
  const StockPrey* prey;
};

#endif
