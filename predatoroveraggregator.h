#ifndef predatoroveraggregator_h
#define predatoroveraggregator_h

#include "agebandm.h"
#include "intmatrix.h"
#include "predatorptrvector.h"

class PredatorOverAggregator;

class PredatorOverAggregator {
public:
  PredatorOverAggregator(const Predatorptrvector& predators,
    const intmatrix& Areas, const LengthGroupDivision* const predLgrpDiv);
  void Sum();
  const doublematrix& ReturnSum() const;
protected:
  Predatorptrvector predators;
  intmatrix predConv;  //[pred][predLengthGroup]
  intmatrix areas;
  doublematrix total;
};

#endif
