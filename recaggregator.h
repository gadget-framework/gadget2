#ifndef recaggregator_h
#define recaggregator_h

#include "areatime.h"
#include "conversionindex.h"
#include "agebandmatrix.h"
#include "agebandmatrixptrvector.h"
#include "charptrvector.h"
#include "fleetptrvector.h"
#include "stockptrvector.h"
#include "intmatrix.h"
#include "conversionindexptrvector.h"
#include "tags.h"

class RecAggregator;

class RecAggregator {
public:
  RecAggregator(const FleetPtrVector& Fleets, const StockPtrVector& Stocks,
    LengthGroupDivision* const Lgrpdiv, const IntMatrix& Areas,
    const IntMatrix& Ages, Tags* tag);
  ~RecAggregator();
  int NoLengthGroups() const { return LgrpDiv->NoLengthGroups(); };
  int NoAgeGroups() const { return ages.Nrow(); };
  int NoAreaGroups() const { return areas.Nrow(); };
  LengthGroupDivision* ReturnLgrpDiv() const { return LgrpDiv; };
  void Sum(const TimeClass* const TimeInfo);
  void Print(ofstream &outfile) const;
  const AgeBandMatrixPtrVector& AgeLengthDist() const { return totalcatch; };
private:
  FleetPtrVector fleets;
  StockPtrVector stocks;
  Tags* taggingExp;
  LengthGroupDivision* LgrpDiv;
  IntMatrix areas;
  IntMatrix ages;
  ConversionIndexPtrVector CI;
  AgeBandMatrixPtrVector totalcatch;
};

#endif
