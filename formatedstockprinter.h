#ifndef formatedstockprinter_h
#define formatedstockprinter_h

#include "printer.h"
#include "gadget.h"

class FormatedStockPrinter;
class StockAggregator;

/*  FormatedStockPrinter
 *
 *  Purpose: Print N and W from stocks.
 *
 *  Input: Uses the same input format as StockPrinter with the addition
 *         of a new keyword, "all" after "ages" or "lengths"
 *
 *  Usage: Same as StockPrinter
 */
class FormatedStockPrinter : public Printer {
public:
  FormatedStockPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual ~FormatedStockPrinter();
  void SetStock(StockPtrVector& stockvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  IntMatrix areas;
  IntMatrix ages;
  CharPtrVector areaindex;
  CharPtrVector ageindex;
  IntVector agevector;
  LengthGroupDivision* LgrpDiv;
  CharPtrVector stocknames;
  StockAggregator* aggregator;
  ofstream noutfile;
  ofstream woutfile;
};

#endif
