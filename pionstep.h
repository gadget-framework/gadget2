#ifndef pionstep_h
#define pionstep_h

#include "commentstream.h"
#include "charptrvector.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "doublematrixptrvector.h"
#include "sionstep.h"

class PIOnStep;
class PredatorAggregator;
class TimeClass;
class AreaClass;

class PIOnStep : public SIOnStep {
public:
  PIOnStep(CommentStream& infile,  const intvector& areas,
    const doublevector& predatorlengths, const doublevector& preylengths,
    const TimeClass* const TimeInfo, int biomass, const char* arealabel,
    const charptrvector& preylenindex, const charptrvector& predlenindex, const char* datafilename);
  ~PIOnStep();
  virtual void Sum(const TimeClass* const TimeInfo);
  virtual void SetStocks(const Stockptrvector& Stocks) {};
  virtual void SetPredatorsAndPreys(const Predatorptrvector& predators, const Preyptrvector& preys);
protected:
  void ReadPredatorData(CommentStream&, const char*, const charptrvector&, const charptrvector&, const TimeClass*);
  LengthGroupDivision* PredatorLgrpDiv;
  LengthGroupDivision* PreyLgrpDiv;
  int Biomass;
  intvector Areas;
  PredatorAggregator* aggregator;
  int index;
};

#endif
