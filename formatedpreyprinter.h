#ifndef formatedpreyprinter_h
#define formatedpreyprinter_h

#include "commentstream.h"
#include "printer.h"
#include "gadget.h"

class FormatedPreyPrinter;
class TimeClass;
class LengthGroupDivision;
class StockAggregator;
class AreaClass;

class FormatedPreyPrinter : public Printer {
public:
  FormatedPreyPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual ~FormatedPreyPrinter();
  void SetStock(Stockptrvector& stockvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  intvector areas;
  charptrvector areaindex;
  charptrvector stocknames;
  ofstream zoutfile;
  ofstream noutfile;
  ofstream coutfile;
  int printzp;
  int printnp;
  int printcp;
  Stockptrvector stocks;
  const AreaClass* Area;
};
#endif
