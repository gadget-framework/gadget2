#ifndef stockprinter_h
#define stockprinter_h

#include "commentstream.h"
#include "printer.h"
#include "gadget.h"

class StockPrinter;
class TimeClass;
class LengthGroupDivision;
class StockAggregator;
class AreaClass;

class StockPrinter : public Printer {
public:
  StockPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual ~StockPrinter();
  virtual void SetStock(StockPtrVector& stockvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  IntMatrix areas;
  IntMatrix ages;
  CharPtrVector areaindex;
  CharPtrVector ageindex;
  CharPtrVector lenindex;
  CharPtrVector stocknames;
  LengthGroupDivision* LgrpDiv;
  StockAggregator* aggregator;
  ofstream outfile;
};

#endif
