#ifndef predatorindex_h
#define predatorindex_h

#include "commentstream.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "lengthgroup.h"
#include "pionstep.h"

class PredatorIndices;
class PIOnStep;

class PredatorIndices : public Likelihood {
public:
  PredatorIndices(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double w, const char* name);
  virtual ~PredatorIndices();
  virtual void Print(ofstream& outfile) const;
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void SetPredatorsAndPreys(PredatorPtrVector& Predators, PreyPtrVector& Preys);
  virtual void Reset(const Keeper* const keeper);
protected:
  CharPtrVector predatornames;
  CharPtrVector preynames;
  PIOnStep* PI;
  IntMatrix areas;
  char* piname;
};

#endif
