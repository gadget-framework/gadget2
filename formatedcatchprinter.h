#ifndef formatedcatchprinter_h
#define formatedcatchprinter_h

#include "commentstream.h"
#include "catchdistributionptrvector.h"
#include "printer.h"
#include "likelihoodptrvector.h"
#include "readword.h"
#include "gadget.h"

class FormatedCatchPrinter;
class TimeClass;
class LengthGroupDivision;
class StockAggregator;
class AreaClass;

/*  FormatedCatchPrinter
 *
 *  Purpose: Print N and W from stocks.
 *
 *  Input: Uses the same input format as StockPrinter with the addition
 *         of a new keyword, "all" after "ages" or "lengths"
 *
 *  Usage: Same as StockPrinter
 *
 */

class FormatedCatchPrinter : public Printer {
public:
  FormatedCatchPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual ~FormatedCatchPrinter();
  void SetLikely(Likelihoodptrvector& likvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  //doublematrixptrmatrix& timeAgg(doublematrixptrmatrix& m);
  void printCalcBiomassByAgeAndFleet(int yearagg, ostream& outfile, int pr, int wd);
  void printCalcBiomassByFleet(int yearagg, ostream& outfile, int pr, int wd);
  void printObsBiomassByAgeAndFleet(int yearagg, ostream& outfile, int pr, int wd);
  void printObsBiomassByFleet(int yearagg, ostream& outfile, int pr, int wd);
  void printCalcCatchByFleet(int yearagg, ostream& outfile, int pr, int w);
  void printObsCatchByFleet(int yearagg, ostream& outfile, int pr, int w);
  void printCalcCatchByAgeAndFleet(int yearagg, ostream& outfile, int pr, int w);
  void printObsCatchByAgeAndFleet(int yearagg, ostream& outfile, int pr, int w);
  void printTime(ostream& out, int time, int yearagg);
  intvector areas;
  intvector timevec;
  charptrvector catchnames;
  ofstream biofile;
  ofstream numberfile;
  CatchDistributionptrvector catches;
  const AreaClass* Area;
  const TimeClass* Time;
  int ageagg;
  int bioprint;
  int numberprint;
};

#endif
