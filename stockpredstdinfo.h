#ifndef stockpredstdinfo_h
#define stockpredstdinfo_h

#include "abstrpredstdinfo.h"

class StockPredStdInfo;
class AbstrPreyStdInfo;
class PredStdInfoByLength;
class Prey;
class StockPredator;
class StockPrey;
class TimeClass;

class StockPredStdInfo : public AbstrPredStdInfo {
public:
  StockPredStdInfo(const StockPredator* predator, const Prey* prey, const intvector& areas);
  StockPredStdInfo(const StockPredator* predator, const StockPrey* prey, const intvector& areas);
  virtual ~StockPredStdInfo();
  virtual const bandmatrix& NconsumptionByLength(int area) const;
  virtual const bandmatrix& BconsumptionByLength(int area) const;
  virtual const bandmatrix& MortalityByLength(int area) const;
  virtual void Sum(const TimeClass* const TimeInfo, int area);
private:
  AbstrPreyStdInfo* preyinfo;
  PredStdInfoByLength* predinfo;
  const StockPredator* predator;
  const Prey* prey;
};

#endif
