#ifndef recapture_h
#define recapture_h

#include "likelihood.h"
#include "commentstream.h"
#include "recaggregator.h"
#include "doublematrixptrmatrix.h"
#include "tagptrvector.h"

class Recaptures : public Likelihood {
public:
  Recaptures(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double w, TagPtrVector Tag, const char* name);
  virtual ~Recaptures();
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  virtual void LikelihoodPrint(ofstream& outfile);
  virtual void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
private:
  void readRecaptureData(CommentStream& infile, const TimeClass* const TimeInfo);
  double LikPoisson(const TimeClass* const TimeInfo);
  RecAggregator** aggregator;
  CharPtrVector tagid;
  CharPtrVector fleetnames;
  CharPtrVector areaindex;
  CharPtrVector lenindex;
  IntMatrix areas;
  DoubleVector lengths;
  IntMatrix Years;
  IntMatrix Steps;
  DoubleMatrixPtrMatrix obsRecaptures;
  DoubleMatrixPtrMatrix modelRecaptures;
  LengthGroupDivision* lgrpdiv;
  TagPtrVector tagvec;
  char* tagname;
  int functionnumber;
  char* functionname;
  int numarea;
  int numlen;
};

#endif
