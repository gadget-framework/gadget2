#ifndef fleetpreyaggregator_h
#define fleetpreyaggregator_h

#include "conversion.h"
#include "agebandmatrix.h"
#include "agebandmatrixptrvector.h"
#include "commentstream.h"
#include "charptrvector.h"
#include "fleetptrvector.h"
#include "stockptrvector.h"
#include "intmatrix.h"
#include "conversionindexptrvector.h"

class FleetPreyAggregator;
class TimeClass;
class AreaClass;

class FleetPreyAggregator {
public:
  FleetPreyAggregator(const FleetPtrVector& Fleets, const StockPtrVector& Stocks,
    LengthGroupDivision* const Lgrpdiv, const IntMatrix& Areas,
    const IntMatrix& Ages, const int overconsumtion);
  ~FleetPreyAggregator();
  int NoLengthGroups() const { return numlengths; };
  int NoAgeGroups() const { return ages.Nrow(); };
  int NoAreaGroups() const { return areas.Nrow(); };
  LengthGroupDivision* ReturnLgrpDiv() const { return LgrpDiv; };
  void Sum(const TimeClass* const TimeInfo);
  void Sum(const TimeClass* const TimeInfo, int dummy); //mortality model
  void Print(ofstream &outfile) const;
  const AgeBandMatrixPtrVector& AgeLengthDist() const { return totalcatch; };
  const BandMatrixVector& CatchRatios() const { return catchratios; };
  const IntVector& getMinCol() const { return mincol; };
  const IntVector& getMaxCol() const { return maxcol; };
  int getMinRow() const { return minrow; };
  int getMaxRow() const { return maxrow; };
private:
  FleetPtrVector fleets;
  StockPtrVector stocks;
  LengthGroupDivision* LgrpDiv;
  int numlengths;
  IntMatrix areas;
  IntMatrix ages;
  IntVector mincol;  //Min column for traversing aggregated data
  IntVector maxcol;  //Max column for traversing aggregated data
  int minrow;        //Min row for traversing aggregated data
  int maxrow;        //Max row for traversing aggregated data
  int overconsumption;    //should we take overconsumption into account
  ConversionIndexPtrVector CI;
  AgeBandMatrixPtrVector totalcatch;
  BandMatrixVector catchratios;  //the ratio caught of the stocks by
                                 //the fleets in question, i.e. 1-exp(-F/12)
};

#endif
