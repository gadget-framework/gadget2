#ifndef predatoraggregator_h
#define predatoraggregator_h

#include "agebandmatrix.h"
#include "intmatrix.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "charptrvector.h"

class PredatorAggregator;

class PredatorAggregator {
public:
  //The first constructor is for length structured predators,
  //the second constructor is for age structured predators
  PredatorAggregator(const PredatorPtrVector& predators, const PreyPtrVector& preys,
    const IntMatrix& Areas, const LengthGroupDivision* const predLgrpDiv,
    const LengthGroupDivision* const preyLgrpDiv);
  PredatorAggregator(const CharPtrVector& prednames, PreyPtrVector& Preys,
    const IntMatrix& Areas, const IntVector& ages,
    const LengthGroupDivision* const preyLgrpDiv);
  ~PredatorAggregator() {};
  void Sum();
  void Sum(int dummy); //to be used with the second constructor
  void NumberSum();
  const BandMatrixVector& ReturnSum() const { return total; };
protected:
  PredatorPtrVector predators;
  PreyPtrVector preys;
  IntMatrix predConv;  //[predator][predatorLengthGroup]
  IntMatrix preyConv;  //[prey][preyLengthGroup]
  IntMatrix areas;
  IntVector AreaNr;
  IntMatrix doeseat;  //[predator][prey] -- does predator eat prey?
  BandMatrixVector total;
};

#endif
