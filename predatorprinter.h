#ifndef predatorprinter_h
#define predatorprinter_h

#include "commentstream.h"
#include "printer.h"
#include "gadget.h"

class PredatorPrinter;
class TimeClass;
class LengthGroupDivision;
class PredatorAggregator;
class AreaClass;

class PredatorPrinter : public Printer {
public:
  PredatorPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual ~PredatorPrinter();
  virtual void SetPredAndPrey(PredatorPtrVector& predatorvec, PreyPtrVector& preyvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  IntMatrix areas;
  CharPtrVector areaindex;
  CharPtrVector predlenindex;
  CharPtrVector preylenindex;
  CharPtrVector predatornames;
  CharPtrVector preynames;
  LengthGroupDivision* predLgrpDiv;
  LengthGroupDivision* preyLgrpDiv;
  PredatorAggregator* aggregator;
  ofstream outfile;
};

#endif
