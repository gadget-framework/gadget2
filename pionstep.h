#ifndef pionstep_h
#define pionstep_h

#include "areatime.h"
#include "commentstream.h"
#include "charptrvector.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "sionstep.h"

class PIOnStep;
class PredatorAggregator;

class PIOnStep : public SIOnStep {
public:
  PIOnStep(CommentStream& infile,  const IntVector& areas,
    const DoubleVector& predatorlengths, const DoubleVector& preylengths,
    const TimeClass* const TimeInfo, int biomass, const CharPtrVector& areaindex,
    const CharPtrVector& preylenindex, const CharPtrVector& predlenindex,
    const char* datafilename, const char* name);
  ~PIOnStep();
  virtual void Sum(const TimeClass* const TimeInfo);
  virtual void SetStocks(const StockPtrVector& Stocks) {};
  virtual void SetPredatorsAndPreys(const PredatorPtrVector& predators, const PreyPtrVector& preys);
protected:
  void ReadPredatorData(CommentStream& infile, const CharPtrVector& areaindex,
    const CharPtrVector& predlenindex, const CharPtrVector& preylenindex,
    const TimeClass* TimeInfo, const char* name);
  LengthGroupDivision* PredatorLgrpDiv;
  LengthGroupDivision* PreyLgrpDiv;
  int Biomass;
  PredatorAggregator* aggregator;
  int index;
};

#endif
