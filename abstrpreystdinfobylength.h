#ifndef abstrpreystdinfobylength_h
#define abstrpreystdinfobylength_h

#include "doublematrix.h"
#include "livesonareas.h"

class AbstrPreyStdInfoByLength;
class TimeClass;
class Prey;

class AbstrPreyStdInfoByLength : protected LivesOnAreas {
public:
  AbstrPreyStdInfoByLength(const Prey* p, const intvector& areas);
  virtual ~AbstrPreyStdInfoByLength();
  const doublevector& NconsumptionByLength(int area) const;
  const doublevector& BconsumptionByLength(int area) const;
  const doublevector& MortalityByLength(int area) const;
  virtual void Sum(const TimeClass* const TimeInfo, int area) = 0;
protected:
  doublematrix MortbyLength;
  doublematrix NconbyLength;
  doublematrix BconbyLength;
};

#endif
