#ifndef biomassprinter_h
#define biomassprinter_h

#include "commentstream.h"
#include "printer.h"
#include "gadget.h"

class BiomassPrinter;
class TimeClass;
class LengthGroupDivision;
class StockAggregator;
class AreaClass;

/*  BiomassPrinter
 *
 *  Purpose: Print z, mean_n, and cannibalism from MortPrey
 *
 *  Input:  BiomassPrinter
 *          stocks <s_1> <s_2> ... <s_n>
 *          areas <a_1> <a_2> ... <a_n>
 *              zprintfile <z_name>
 *              nprintfile <meann_name>
 *              cprintfile <cannibalism_name>
 *              yearsandsteps [format of years and steps]
 *
 *  Usage:      Same as StockPrinter
 *
 */

class BiomassPrinter : public Printer {
public:
  BiomassPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual ~BiomassPrinter();
  virtual void SetStock(Stockptrvector& stockvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  intvector areas;
  charptrvector areaindex;
  charptrvector matnames;
  char* immname;
  ofstream printfile;
  Stockptrvector stocks;
  int immindex;
  int minage;
  int maxage;
  const AreaClass* Area;
  int nrofyears;
  int firstyear;
};

#endif
