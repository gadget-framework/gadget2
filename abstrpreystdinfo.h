#ifndef abstrpreystdinfo_h
#define abstrpreystdinfo_h

#include "bandmatrix.h"
#include "livesonareas.h"

class AbstrPreyStdInfo;
class TimeClass;
class Prey;

class AbstrPreyStdInfo : public LivesOnAreas {
public:
  AbstrPreyStdInfo(const Prey* p, const IntVector& areas,
    int minage = 0, int maxage = 0);
  virtual ~AbstrPreyStdInfo();
  virtual const DoubleVector& NconsumptionByLength(int area) const = 0;
  virtual const DoubleVector& BconsumptionByLength(int area) const = 0;
  virtual const DoubleVector& MortalityByLength(int area) const = 0;
  const BandMatrix& NconsumptionByAgeAndLength(int area) const;
  const DoubleIndexVector& NconsumptionByAge(int area) const;
  const BandMatrix& BconsumptionByAgeAndLength(int area) const;
  const DoubleIndexVector& BconsumptionByAge(int area) const;
  const BandMatrix& MortalityByAgeAndLength(int area) const;
  const DoubleIndexVector& MortalityByAge(int area) const;
  virtual void Sum(const TimeClass* const TimeInfo, int area) = 0;
protected:
  BandMatrix NconbyAge;
  BandMatrix BconbyAge;
  BandMatrix MortbyAge;
  BandMatrixVector NconbyAgeAndLength;
  BandMatrixVector BconbyAgeAndLength;
  BandMatrixVector MortbyAgeAndLength;
};

#endif
