#ifndef preyoveraggregator_h
#define preyoveraggregator_h

#include "doublematrix.h"
#include "intmatrix.h"
#include "preyptrvector.h"

class PreyOverAggregator;
class LengthGroupDivision;

class PreyOverAggregator {
public:
  PreyOverAggregator(const Preyptrvector& preys,
    const intmatrix& Areas, const LengthGroupDivision* const preyLgrpDiv);
  void Sum();
  const doublematrix& ReturnSum() const;
protected:
  Preyptrvector preys;
  intmatrix preyConv;  //[prey][preyLengthGroup]
  intmatrix areas;
  doublematrix total;
};

#endif
