#ifndef abstrpredstdinfo_h
#define abstrpredstdinfo_h

#include "bandmatrix.h"
#include "livesonareas.h"
#include "areatime.h"

class AbstrPredStdInfo;

class AbstrPredStdInfo : protected LivesOnAreas {
public:
  AbstrPredStdInfo(const IntVector& areas, int predminage = 0,
    int predmaxage = 0, int preyminage = 0, int preymaxage = 0);
  virtual ~AbstrPredStdInfo() {};
  virtual const BandMatrix& NconsumptionByLength(int area) const = 0;
  virtual const BandMatrix& BconsumptionByLength(int area) const = 0;
  virtual const BandMatrix& MortalityByLength(int area) const = 0;
  const BandMatrix& NconsumptionByAge(int area) const;
  const BandMatrix& BconsumptionByAge(int area) const;
  const BandMatrix& MortalityByAge(int area) const;
  virtual void Sum(const TimeClass* const TimeInfo, int area) = 0;
protected:
  BandMatrixVector NconbyAge;
  BandMatrixVector BconbyAge;
  BandMatrixVector MortbyAge;
};

#endif
