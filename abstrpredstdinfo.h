#ifndef abstrpredstdinfo_h
#define abstrpredstdinfo_h

#include "bandmatrix.h"
#include "livesonareas.h"

class AbstrPredStdInfo;
class TimeClass;

class AbstrPredStdInfo : protected LivesOnAreas {
public:
  AbstrPredStdInfo(const intvector& areas, int predminage = 0,
    int predmaxage = 0, int preyminage = 0, int preymaxage = 0);
  virtual ~AbstrPredStdInfo();
  virtual const bandmatrix& NconsumptionByLength(int area) const = 0;
  virtual const bandmatrix& BconsumptionByLength(int area) const = 0;
  virtual const bandmatrix& MortalityByLength(int area) const = 0;
  const bandmatrix& NconsumptionByAge(int area) const;
  const bandmatrix& BconsumptionByAge(int area) const;
  const bandmatrix& MortalityByAge(int area) const;
  virtual void Sum(const TimeClass* const TimeInfo, int area) = 0;
protected:
  bandmatrixvector NconbyAge;
  bandmatrixvector BconbyAge;
  bandmatrixvector MortbyAge;
};

#endif
