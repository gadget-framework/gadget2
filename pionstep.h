#ifndef pionstep_h
#define pionstep_h

#include "commentstream.h"
#include "charptrvector.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "sionstep.h"

class PIOnStep;
class PredatorAggregator;
class TimeClass;
class AreaClass;

class PIOnStep : public SIOnStep {
public:
  PIOnStep(CommentStream& infile,  const IntVector& areas,
    const DoubleVector& predatorlengths, const DoubleVector& preylengths,
    const TimeClass* const TimeInfo, int biomass, const char* arealabel,
    const CharPtrVector& preylenindex, const CharPtrVector& predlenindex, const char* datafilename);
  ~PIOnStep();
  virtual void Sum(const TimeClass* const TimeInfo);
  virtual void SetStocks(const StockPtrVector& Stocks) {};
  virtual void SetPredatorsAndPreys(const PredatorPtrVector& predators, const PreyPtrVector& preys);
  //virtual void Print(ofstream& outfile) const;
protected:
  void ReadPredatorData(CommentStream&, const char*, const CharPtrVector&, const CharPtrVector&, const TimeClass*);
  LengthGroupDivision* PredatorLgrpDiv;
  LengthGroupDivision* PreyLgrpDiv;
  int Biomass;
  IntVector Areas;
  PredatorAggregator* aggregator;
  int index;
};

#endif
