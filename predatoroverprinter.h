#ifndef predatoroverprinter_h
#define predatoroverprinter_h

#include "predatorptrvector.h"
#include "printer.h"
#include "gadget.h"

class PredatorOverPrinter;
class TimeClass;
class LengthGroupDivision;
class PredatorOverAggregator;
class AreaClass;

class PredatorOverPrinter : public Printer {
public:
  PredatorOverPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual ~PredatorOverPrinter();
  void SetPredator(Predatorptrvector& predatorvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  intmatrix areas;
  charptrvector areaindex;
  charptrvector lenindex;
  charptrvector predatornames;
  LengthGroupDivision* predLgrpDiv;
  PredatorOverAggregator* aggregator;
  ofstream outfile;
};

#endif
