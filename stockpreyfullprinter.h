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
  virtual void SetStock(Stockptrvector& stockvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  char* stockname;
  LengthGroupDivision* LgrpDiv;
  intvector areas;
  intvector outerareas;
  StockPreyStdInfo* preyinfo;
  ofstream outfile;
};

#endif
