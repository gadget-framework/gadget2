#ifndef preystdinfo_h
#define preystdinfo_h

#include "abstrpreystdinfo.h"
#include "preystdinfobylength.h"

class PreyStdInfo;
class TimeClass;
class Prey;

class PreyStdInfo : public AbstrPreyStdInfo {
public:
  PreyStdInfo(const Prey* prey, const intvector& areas);
  virtual ~PreyStdInfo();
  virtual const doublevector& NconsumptionByLength(int area) const;
  virtual const doublevector& BconsumptionByLength(int area) const;
  virtual const doublevector& MortalityByLength(int area) const;
  virtual void Sum(const TimeClass* const TimeInfo, int area);
private:
  PreyStdInfoByLength PSIByLength;
  const Prey* prey;
};

#endif
