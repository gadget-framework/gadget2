#ifndef catchintons_h
#define catchintons_h

#include "likelihood.h"
#include "actionattimes.h"
#include "commentstream.h"
#include "charptrvector.h"
#include "intmatrix.h"

class Keeper;
class TimeClass;
class AreaClass;

class CatchInTons : public Likelihood {
public:
  CatchInTons(CommentStream& infile, const AreaClass* const areas,
    const TimeClass* const timeinfo, double likweight);
  void Reset(const Keeper* const keeper);
  void Print(ofstream& outfile) const;
  void SetFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
  void AddToLikelihood(const TimeClass* const TimeInfo);
  ~CatchInTons();
private:
  void ReadCatchInTonsData(CommentStream&, const TimeClass*, int);
  double SumOfSquares(double obs, double mod);
  CharPtrVector fleetnames;
  CharPtrVector stocknames;
  CharPtrVector areaindex;
  IntMatrix preyindex;
  FleetPtrVector fleets;
  StockPtrVector stocks;
  IntMatrix areas;
  DoubleMatrix DataCatch;
  DoubleMatrix ModelCatch;
  int yearly;
  double epsilon;
  ActionAtTimes AAT;
  IntVector Years;
  IntVector Steps;
  int timeindex;
  int functionnumber;
  int debug_print;
  char* functionname;
};

#endif
