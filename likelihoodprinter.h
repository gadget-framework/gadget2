#ifndef likelihoodprinter_h
#define likelihoodprinter_h

#include "printer.h"
#include "readword.h"

class LikelihoodPrinter : public Printer {
public:
  LikelihoodPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual ~LikelihoodPrinter();
  void setLikely(LikelihoodPtrVector& likvec);
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
