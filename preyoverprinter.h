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
  void SetPrey(Preyptrvector& preyvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  intmatrix areas;
  charptrvector areaindex;
  charptrvector lenindex;
  charptrvector preynames;
  LengthGroupDivision* preyLgrpDiv;
  PreyOverAggregator* aggregator;
  ofstream outfile;
};

#endif
