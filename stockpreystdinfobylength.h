#ifndef stockpreystdinfobylength_h
#define stockpreystdinfobylength_h

#include "abstrpreystdinfobylength.h"

class StockPreyStdInfoByLength;
class StockPrey;

class StockPreyStdInfoByLength : public AbstrPreyStdInfoByLength {
public:
  StockPreyStdInfoByLength(const StockPrey* prey, const intvector& areas);
  virtual ~StockPreyStdInfoByLength();
  virtual void Sum(const TimeClass* const TimeInfo, int area);
private:
  const StockPrey* prey;
};

#endif
