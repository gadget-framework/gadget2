#ifndef formatedpreyprinter_h
#define formatedpreyprinter_h

#include "printer.h"
#include "gadget.h"

class FormatedPreyPrinter;
class StockAggregator;

class FormatedPreyPrinter : public Printer {
public:
  FormatedPreyPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual ~FormatedPreyPrinter();
  void SetStock(StockPtrVector& stockvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  IntVector areas;
  CharPtrVector areaindex;
  CharPtrVector stocknames;
  ofstream zoutfile;
  ofstream noutfile;
  ofstream coutfile;
  int printzp;
  int printnp;
  int printcp;
  StockPtrVector stocks;
  const AreaClass* Area;
};
#endif
