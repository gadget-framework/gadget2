#ifndef predstdinfo_h
#define predstdinfo_h

#include "abstrpredstdinfo.h"

class PredStdInfo;
class AbstrPreyStdInfo;
class PopPredator;
class PredStdInfoByLength;
class Prey;
class StockPrey;
class TimeClass;

class PredStdInfo : public AbstrPredStdInfo {
public:
  PredStdInfo(const PopPredator* predator, const Prey* prey, const intvector& areas);
  PredStdInfo(const PopPredator* predator, const StockPrey* prey, const intvector& areas);
  virtual ~PredStdInfo();
  virtual const bandmatrix& NconsumptionByLength(int area) const;
  virtual const bandmatrix& BconsumptionByLength(int area) const;
  virtual const bandmatrix& MortalityByLength(int area) const;
  virtual void Sum(const TimeClass* const TimeInfo, int area);
private:
  AbstrPreyStdInfo* preyinfo;
  PredStdInfoByLength* predinfo;
  const PopPredator* predator;
  const Prey* prey;
};

#endif
