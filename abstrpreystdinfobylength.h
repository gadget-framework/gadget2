#ifndef abstrpreystdinfobylength_h
#define abstrpreystdinfobylength_h

#include "doublematrix.h"
#include "livesonareas.h"
#include "areatime.h"

class AbstrPreyStdInfoByLength;
class Prey;

class AbstrPreyStdInfoByLength : protected LivesOnAreas {
public:
  AbstrPreyStdInfoByLength(const Prey* p, const IntVector& areas);
  virtual ~AbstrPreyStdInfoByLength();
  const DoubleVector& NconsumptionByLength(int area) const;
  const DoubleVector& BconsumptionByLength(int area) const;
  const DoubleVector& MortalityByLength(int area) const;
  virtual void Sum(const TimeClass* const TimeInfo, int area) = 0;
protected:
  DoubleMatrix MortbyLength;
  DoubleMatrix NconbyLength;
  DoubleMatrix BconbyLength;
};

#endif
