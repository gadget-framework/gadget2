#ifndef abstrpreystdinfo_h
#define abstrpreystdinfo_h

#include "bandmatrix.h"
#include "livesonareas.h"

class AbstrPreyStdInfo;
class TimeClass;
class Prey;

class AbstrPreyStdInfo : public LivesOnAreas {
public:
  AbstrPreyStdInfo(const Prey* p, const intvector& areas,
    int minage = 0, int maxage = 0);
  virtual ~AbstrPreyStdInfo();
  virtual const doublevector& NconsumptionByLength(int area) const = 0;
  virtual const doublevector& BconsumptionByLength(int area) const = 0;
  virtual const doublevector& MortalityByLength(int area) const = 0;
  const bandmatrix& NconsumptionByAgeAndLength(int area) const;
  const doubleindexvector& NconsumptionByAge(int area) const;
  const bandmatrix& BconsumptionByAgeAndLength(int area) const;
  const doubleindexvector& BconsumptionByAge(int area) const;
  const bandmatrix& MortalityByAgeAndLength(int area) const;
  const doubleindexvector& MortalityByAge(int area) const;
  virtual void Sum(const TimeClass* const TimeInfo, int area) = 0;
protected:
  bandmatrix NconbyAge;
  bandmatrix BconbyAge;
  bandmatrix MortbyAge;
  bandmatrixvector NconbyAgeAndLength;
  bandmatrixvector BconbyAgeAndLength;
  bandmatrixvector MortbyAgeAndLength;
};

#endif
