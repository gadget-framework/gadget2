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
  void SetFleetsAndStocks(Fleetptrvector& Fleets, Stockptrvector& Stocks);
  void AddToLikelihood(const TimeClass* const TimeInfo);
  ~CatchInTons();
private:
  void ReadCatchInTonsData(CommentStream&, const TimeClass*, int);
  double SumOfSquares(double obs, double mod);
  charptrvector fleetnames;
  charptrvector stocknames;
  charptrvector areaindex;
  intmatrix preyindex;
  Fleetptrvector fleets;
  Stockptrvector stocks;
  intmatrix areas;
  doublematrix DataCatch;
  doublematrix ModelCatch;
  int yearly;
  double epsilon;
  ActionAtTimes AAT;
  intvector Years;
  intvector Steps;
  int timeindex;
  int functionnumber;
  int debug_print;
  char* functionname;
};

#endif
