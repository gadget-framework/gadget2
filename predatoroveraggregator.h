#ifndef predatoroveraggregator_h
#define predatoroveraggregator_h

#include "agebandmatrix.h"
#include "intmatrix.h"
#include "predatorptrvector.h"

class PredatorOverAggregator;

class PredatorOverAggregator {
public:
  PredatorOverAggregator(const PredatorPtrVector& predators,
    const IntMatrix& Areas, const LengthGroupDivision* const predLgrpDiv);
  void Sum();
  const DoubleMatrix& ReturnSum() const;
protected:
  PredatorPtrVector predators;
  IntMatrix predConv;  //[pred][predLengthGroup]
  IntMatrix areas;
  DoubleMatrix total;
};

#endif
