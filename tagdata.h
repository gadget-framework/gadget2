#ifndef tagdata_h
#define tagdata_h

#include "likelihood.h"
#include "commentstream.h"
#include "recaggregator.h"
#include "actionattimes.h"
#include "tagptrvector.h"

class TagData : public Likelihood {
public:
  TagData(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double w, TagPtrVector Tag);
  virtual ~TagData();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const {};
  virtual void SetFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
private:
  void ReadRecaptureData(CommentStream& infile,
    const TimeClass* const TimeInfo, CharPtrVector areaindex);
  RecAggregator** aggregator;
  CharPtrVector fleetnames;
  CharPtrVector stocknames;
  CharPtrVector tagid;
  IntVector areas;
  IntVector index;
  IntMatrix Years;
  IntMatrix Steps;
  IntMatrix recTime;
  IntMatrix recAreas;
  IntMatrix recAge;
  IntMatrix recFleet;
  IntMatrix recMaturity;
  DoubleMatrix tagLength;
  DoubleMatrix recLength;
  IntMatrix recaptures;
  IntMatrix p;
  TagPtrVector tagvec;
};

#endif
