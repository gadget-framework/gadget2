#ifndef preyoverprinter_h
#define preyoverprinter_h

#include "printer.h"
#include "commentstream.h"
#include "gadget.h"

class PreyOverPrinter;
class TimeClass;
class LengthGroupDivision;
class PreyOverAggregator;
class AreaClass;

class PreyOverPrinter : public Printer {
public:
  PreyOverPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual ~PreyOverPrinter();
  void SetPrey(PreyPtrVector& preyvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  IntMatrix areas;
  CharPtrVector areaindex;
  CharPtrVector lenindex;
  CharPtrVector preynames;
  LengthGroupDivision* preyLgrpDiv;
  PreyOverAggregator* aggregator;
  ofstream outfile;
};

#endif
