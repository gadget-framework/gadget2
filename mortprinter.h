#ifndef mortprinter_h
#define mortprinter_h

#include "commentstream.h"
#include "printer.h"
#include "lenstock.h"
#include "gadget.h"

class MortPrinter;
class TimeClass;
class LengthGroupDivision;
class StockAggregator;
class AreaClass;

/*  MortPrinter
 *
 *  Purpose: Print z, mean_n, and cannibalism from MortPrey
 *
 *  Input: MortPrinter
 *     stocks <s_1> <s_2> ... <s_n>
 *     areas <a_1> <a_2> ... <a_n>
 *         zprintfile <z_name>
 *         nprintfile <meann_name>
 *         cprintfile <cannibalism_name>
 *         yearsandsteps [format of years and steps]
 *
 *  Usage: Same as StockPrinter
 */

class MortPrinter : public Printer {
public:
  MortPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual ~MortPrinter();
  virtual void SetStock(StockPtrVector& stockvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  const AreaClass* Area;
  IntVector areas;
  CharPtrVector areaindex;
  CharPtrVector stocknames;
  StockPtrVector stocks;
  ofstream outfile;
  int printf;
  int printm1;
  int printm2;
  int printn;
  int firstyear;
  int nrofyears;
  int minage;
  int maxage;
  DoubleMatrix* sumF;
  DoubleMatrix* sumM1;
  DoubleMatrix* sumM2;
  DoubleMatrix* sumN;
  DoubleMatrix* totalNbar;
};

#endif
