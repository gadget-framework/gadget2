#ifndef tagdata_h
#define tagdata_h

#include "likelihood.h"
#include "commentstream.h"
#include "fleetpreyaggregator.h"
#include "actionattimes.h"

class TimeClass;
class AreaClass;

class TagData : public Likelihood {
public:
  TagData(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double likweight);
  virtual ~TagData();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  virtual void LikelihoodPrint(ofstream& outfile) const {};
  //AJ 06.06.00 Adding function
  virtual void SetFleetsAndStocks(Fleetptrvector& Fleets, Stockptrvector& Stocks);
private:
  void ReadRecaptureData(CommentStream&, const TimeClass*, charptrvector);
  double LikBinomial();
  FleetPreyAggregator* aggregator;
  charptrvector fleetnames;
  char* stockname;
  intvector areas;
  int index;
  intvector Years;
  intvector Steps;
  intvector Areas;
  intvector recaptures;
  double n_gt_x;
};

#endif
