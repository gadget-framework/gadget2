#ifndef boundlikelihood_h
#define boundlikelihood_h

#include "likelihood.h"
#include "commentstream.h"
#include "intvector.h"
#include "doublevector.h"
#include "errorhandler.h"
#include "parameter.h"

class BoundLikelihood : public Likelihood {
public:
  BoundLikelihood(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const Keeper* const keeper, double w);
  virtual ~BoundLikelihood();
  void Reset(const Keeper* const keeper);
  virtual void AddToLikelihood(const TimeClass* const TimeInfo) {};
  virtual void Print(ofstream&) const {};
  virtual void AddToLikelihoodTimeAndKeeper(const TimeClass* const TimeInfo, Keeper* const keeper);
protected:
  IntVector switchnr;
  DoubleVector upperbound;
  DoubleVector lowerbound;
  DoubleVector powers;
  DoubleVector lowerweights;
  DoubleVector upperweights;
  DoubleVector likelihoods;
  double defLW;     // default value for the lower weight
  double defUW;     // default value for the upper weight
  double defPower;  // default value for the power
private:
  int checkInitialised;
};

#endif
