#ifndef catchintons_h
#define catchintons_h

#include "likelihood.h"
#include "actionattimes.h"
#include "commentstream.h"
#include "charptrvector.h"
#include "intmatrix.h"

class CatchInTons : public Likelihood {
public:
  CatchInTons(CommentStream& infile, const AreaClass* const areas,
    const TimeClass* const timeinfo, double w);
  ~CatchInTons();
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  virtual void LikelihoodPrint(ofstream& outfile);
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  void SetFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
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
