#ifndef stockstdprinter_h
#define stockstdprinter_h

#include "commentstream.h"
#include "printer.h"
#include "gadget.h"

class StockStdPrinter;
class TimeClass;
class StockAggregator;
class StockPreyStdInfo;
class AreaClass;
class LengthGroupDivision;

class StockStdPrinter : public Printer {
public:
  StockStdPrinter(CommentStream& infile,
    const AreaClass* const Area, const  TimeClass* const TimeInfo);
  virtual ~StockStdPrinter();
  virtual void SetStock(StockPtrVector& stockvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  char* stockname;
  LengthGroupDivision* LgrpDiv;
  IntVector areas;
  IntVector outerareas;
  int minage;
  int maxage;
  StockAggregator* aggregator;
  StockPreyStdInfo* preyinfo;
  ofstream outfile;
  double Scale;
};

#endif
