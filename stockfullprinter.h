#ifndef stockfullprinter_h
#define stockfullprinter_h

#include "commentstream.h"
#include "printer.h"
#include "gadget.h"

class StockFullPrinter;
class TimeClass;
class LengthGroupDivision;
class StockAggregator;
class AreaClass;

class StockFullPrinter : public Printer {
public:
  StockFullPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual ~StockFullPrinter();
  virtual void SetStock(Stockptrvector& stockvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  intvector areas;
  intvector outerareas;
  int minage;
  int maxage;
  char* stockname;
  StockAggregator* aggregator;
  LengthGroupDivision* LgrpDiv;
  ofstream outfile;
};

#endif
