#ifndef likelihoodprinter_h
#define likelihoodprinter_h

#include "commentstream.h"
#include "printer.h"
#include "readword.h"
#include "gadget.h"

class LikelihoodPrinter;
class TimeClass;
class LengthGroupDivision;
class StockAggregator;
class AreaClass;

/* LikelihoodPrinter
 *
 * Purpose: Print likelihood information by age and length on step/year
 *
 * Input:
 *
 * Usage:
 */

class LikelihoodPrinter : public Printer {
public:
  LikelihoodPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual ~LikelihoodPrinter();
  void SetLikely(LikelihoodPtrVector& likvec);
  virtual void Print(const TimeClass* const TimeInfo);
protected:
  ofstream catchFile;
  ofstream surveyFile;
  ofstream stomachFile;
  int printCatch;
  int printSurvey;
  int printStomach;
  LikelihoodPtrVector catchvec;
  LikelihoodPtrVector stomachvec;
  LikelihoodPtrVector surveyvec;
};

#endif
