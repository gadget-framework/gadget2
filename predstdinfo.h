#ifndef predstdinfo_h
#define predstdinfo_h

#include "areatime.h"
#include "abstrpredstdinfo.h"

class PredStdInfo;
class AbstrPreyStdInfo;
class PopPredator;
class PredStdInfoByLength;
class Prey;
class StockPrey;

class PredStdInfo : public AbstrPredStdInfo {
public:
  PredStdInfo(const PopPredator* predator, const Prey* prey, const IntVector& areas);
  PredStdInfo(const PopPredator* predator, const StockPrey* prey, const IntVector& areas);
  virtual ~PredStdInfo();
  virtual const BandMatrix& NconsumptionByLength(int area) const;
  virtual const BandMatrix& BconsumptionByLength(int area) const;
  virtual const BandMatrix& MortalityByLength(int area) const;
  virtual void Sum(const TimeClass* const TimeInfo, int area);
private:
  AbstrPreyStdInfo* preyinfo;
  PredStdInfoByLength* predinfo;
  const PopPredator* predator;
  const Prey* prey;
};

#endif
