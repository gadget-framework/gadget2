#ifndef preyoveraggregator_h
#define preyoveraggregator_h

#include "doublematrix.h"
#include "intmatrix.h"
#include "preyptrvector.h"

class PreyOverAggregator;
class LengthGroupDivision;

class PreyOverAggregator {
public:
  PreyOverAggregator(const PreyPtrVector& preys,
    const IntMatrix& Areas, const LengthGroupDivision* const preyLgrpDiv);
  void Sum();
  const DoubleMatrix& ReturnSum() const;
protected:
  PreyPtrVector preys;
  IntMatrix preyConv;  //[prey][preyLengthGroup]
  IntMatrix areas;
  DoubleMatrix total;
};

#endif
