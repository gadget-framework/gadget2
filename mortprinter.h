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
  virtual void SetStock(Stockptrvector& stockvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  const AreaClass* Area;
  intvector areas;
  charptrvector areaindex;
  charptrvector stocknames;
  Stockptrvector stocks;
  ofstream outfile;
  int printf;
  int printm1;
  int printm2;
  int printn;
  int firstyear;
  int nrofyears;
  int minage;
  int maxage;
  doublematrix* sumF;
  doublematrix* sumM1;
  doublematrix* sumM2;
  doublematrix* sumN;
  doublematrix* totalNbar;
};

#endif
