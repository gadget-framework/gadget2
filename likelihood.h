#ifndef likelihood_h
#define likelihood_h

#include "areatime.h"
#include "stockptrvector.h"
#include "fleetptrvector.h"
#include "printinfo.h"
#include "gadget.h"

class Ecosystem;
class TimeClass;
class Keeper;

enum LikelihoodType { SURVEYINDICESLIKELIHOOD = 1, UNDERSTOCKINGLIKELIHOOD,
  CATCHDISTRIBUTIONLIKELIHOOD, CATCHSTATISTICSLIKELIHOOD, STOMACHCONTENTLIKELIHOOD,
  TAGLIKELIHOOD, STOCKDISTRIBUTIONLIKELIHOOD, PREDATORINDICESLIKELIHOOD,
  MIGRATIONPENALTYLIKELIHOOD, RANDOMWALKLIKELIHOOD, BOUNDLIKELIHOOD,
  LOGCATCHLIKELIHOOD, LOGSURVEYLIKELIHOOD, AGGREGATEDCDLIKELIHOOD, CATCHINTONSLIKELIHOOD };

class Likelihood {
public:
  Likelihood(LikelihoodType T, double w = 0) : likelihood(0), weight(w), type(T) {};
  virtual ~Likelihood() {};
  virtual void AddToLikelihood(const TimeClass* const TimeInfo) = 0;
  virtual void AddToLikelihoodTimeAndKeeper(const TimeClass* const TimeInfo, Keeper* const Keeper) {};
  virtual void Reset(const Keeper* const keeper) { likelihood = 0; };
  virtual void Print(ofstream& outfile) const = 0;
  virtual void LikelihoodPrint(ofstream& outfile) const { outfile << "weight " << weight << endl; };
  virtual void LikelihoodPrintKeeper(ofstream& outfile, const Keeper* const Keeper) const {};
  virtual void printMoreLikInfo(ofstream& outfile) const {};
  double ReturnLikelihood() const { return weight * likelihood; };
  LikelihoodType Type() const { return type; };
  double UnweightedLikelihood() const { return likelihood; };
  double Weight() const { return weight; };
  virtual void printHeader(ofstream&, const PrintInfo&) {};
  virtual void print(ofstream&, const TimeClass& time, const PrintInfo& print) {};
  virtual void SetFleetsAndStocks(Fleetptrvector& Fleets, Stockptrvector& Stocks) {};
protected:
  double likelihood;
private:
  double weight;
  LikelihoodType type;
};

#endif
