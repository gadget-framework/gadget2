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
    const TimeClass* const timeinfo, double weight);
  ~CatchInTons();
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  virtual void LikelihoodPrint(ofstream& outfile);
  virtual void addLikelihood(const TimeClass* const TimeInfo);
  void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
private:
  void readCatchInTonsData(CommentStream& infile, const TimeClass* TimeInfo, int numarea);
  double calcLikSumSquares(const TimeClass* const TimeInfo);
  CharPtrVector fleetnames;
  CharPtrVector stocknames;
  CharPtrVector areaindex;
  IntMatrix preyindex;
  FleetPtrVector fleets;
  StockPtrVector stocks;
  IntMatrix areas;
  DoubleMatrix DataCatch;
  DoubleMatrix ModelCatch;
  DoubleMatrix Likelihoodvalues;
  int yearly;
  double epsilon;
  ActionAtTimes AAT;
  IntVector Years;
  IntVector Steps;
  int timeindex;
  int functionnumber;
  char* functionname;
};

#endif
