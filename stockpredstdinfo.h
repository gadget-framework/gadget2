#ifndef stockpredstdinfo_h
#define stockpredstdinfo_h

#include "areatime.h"
#include "abstrpredstdinfo.h"

class StockPredStdInfo;
class AbstrPreyStdInfo;
class PredStdInfoByLength;
class Prey;
class StockPredator;
class StockPrey;

class StockPredStdInfo : public AbstrPredStdInfo {
public:
  StockPredStdInfo(const StockPredator* predator, const Prey* prey, const IntVector& areas);
  StockPredStdInfo(const StockPredator* predator, const StockPrey* prey, const IntVector& areas);
  virtual ~StockPredStdInfo();
  virtual const BandMatrix& NconsumptionByLength(int area) const;
  virtual const BandMatrix& BconsumptionByLength(int area) const;
  virtual const BandMatrix& MortalityByLength(int area) const;
  virtual void Sum(const TimeClass* const TimeInfo, int area);
private:
  AbstrPreyStdInfo* preyinfo;
  PredStdInfoByLength* predinfo;
  const StockPredator* predator;
  const Prey* prey;
};

#endif
