#ifndef predatorindex_h
#define predatorindex_h

#include "commentstream.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "conversion.h"
#include "pionstep.h"

class PredatorIndices;
class TimeClass;
class AreaClass;
class PIOnStep;

class PredatorIndices : public Likelihood {
public:
  PredatorIndices(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double w);
  virtual ~PredatorIndices();
  virtual void Print(ofstream& outfile) const {};
  virtual void LikelihoodPrint(ofstream& outfile) const {};
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void SetPredatorsAndPreys(Predatorptrvector& Predators, Preyptrvector& Preys);
  virtual void Reset(const Keeper* const keeper);
protected:
  charptrvector predatornames;
  charptrvector preynames;
  PIOnStep* PI;
  intvector areas;
};

#endif
