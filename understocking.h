#ifndef understocking_h
#define understocking_h

#include "likelihood.h"
#include "actionattimes.h"
#include "commentstream.h"
#include "intmatrix.h"
#include "charptrvector.h"

class UnderStocking : public Likelihood {
public:
  UnderStocking(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double weight);
  virtual ~UnderStocking();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const {};
  void SetFleets(FleetPtrVector& Fleets);
private:
  CharPtrVector fleetnames;
  FleetPtrVector fleets;
  IntMatrix areas;
  CharPtrVector areaindex;
  ActionAtTimes AAT;
  double powercoeff;
};

#endif
