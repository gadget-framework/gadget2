#ifndef stockpreyfullprinter_h
#define stockpreyfullprinter_h

#include "commentstream.h"
#include "printer.h"
#include "gadget.h"

class StockPreyFullPrinter;
class TimeClass;
class StockPreyStdInfo;
class AreaClass;
class LengthGroupDivision;

class StockPreyFullPrinter : public Printer {
public:
  StockPreyFullPrinter(CommentStream& infile,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual ~StockPreyFullPrinter();
  virtual void SetStock(StockPtrVector& stockvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  char* stockname;
  IntVector areas;
  IntVector outerareas;
  StockPreyStdInfo* preyinfo;
  ofstream outfile;
};

#endif
