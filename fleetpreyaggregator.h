#ifndef fleetpreyaggregator_h
#define fleetpreyaggregator_h

#include "conversion.h"
#include "agebandm.h"
#include "commentstream.h"
#include "charptrvector.h"
#include "fleetptrvector.h"
#include "stockptrvector.h"
#include "intmatrix.h"
#include "conversionindexptrvector.h"
#include "doublematrixptrvector.h"

class FleetPreyAggregator;
class TimeClass;
class AreaClass;

class FleetPreyAggregator {
public:
  FleetPreyAggregator(const Fleetptrvector& Fleets, const Stockptrvector& Stocks,
    LengthGroupDivision* const Lgrpdiv, const intmatrix& Areas,
    const intmatrix& Ages, const int overconsumtion);
  ~FleetPreyAggregator();
  int NoLengthGroups() const { return numlengths; };
  int NoAgeGroups() const { return ages.Nrow(); };
  int NoAreaGroups() const { return areas.Nrow(); };
  LengthGroupDivision* ReturnLgrpDiv() const { return LgrpDiv; };
  void Sum(const TimeClass* const TimeInfo);
  void Sum(const TimeClass* const TimeInfo, int dummy); //mortality model
  void Print(ofstream &outfile) const;
  const Agebandmatrixvector& AgeLengthDist() const { return totalcatch; };
  const bandmatrixvector& CatchRatios() const { return catchratios; };
  const intvector& getMinCol() const { return mincol; };
  const intvector& getMaxCol() const { return maxcol; };
  int getMinRow() const { return minrow; };
  int getMaxRow() const { return maxrow; };
private:
  Fleetptrvector fleets;
  Stockptrvector stocks;
  LengthGroupDivision* LgrpDiv;
  int numlengths;
  intmatrix areas;
  intmatrix ages;
  intvector mincol;  //Min column for traversing aggregated data
  intvector maxcol;  //Max column for traversing aggregated data
  int minrow;        //Min row for traversing aggregated data
  int maxrow;        //Max row for traversing aggregated data
  int overconsumption;    //should we take overconsumption into account
  ConversionIndexptrvector CI;
  Agebandmatrixvector totalcatch;
  bandmatrixvector catchratios;  //the ratio caught of the stocks by
                                 //the fleets in question, i.e. 1-exp(-F/12)
};

#endif
