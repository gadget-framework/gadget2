#ifndef understocking_h
#define understocking_h

#include "likelihood.h"
#include "actionattimes.h"
#include "commentstream.h"
#include "intmatrix.h"
#include "charptrvector.h"

class TimeClass;
class AreaClass;

class UnderStocking : public Likelihood {
public:
  UnderStocking(CommentStream&, const AreaClass* const, const TimeClass* const, double likweight);
  virtual ~UnderStocking();
  virtual void AddToLikelihood(const TimeClass* const);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream&) const {};
  void SetFleets(Fleetptrvector&);
private:
  charptrvector fleetnames;
  Fleetptrvector fleets;
  intmatrix areas;
  charptrvector areaindex;
  ActionAtTimes aat;
  double powercoeff;
};

#endif
