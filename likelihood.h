#ifndef likelihood_h
#define likelihood_h

#include "areatime.h"
#include "stockptrvector.h"
#include "fleetptrvector.h"
#include "printinfo.h"
#include "gadget.h"

class Ecosystem;
class Keeper;

enum LikelihoodType { SURVEYINDICESLIKELIHOOD = 1, UNDERSTOCKINGLIKELIHOOD,
  CATCHDISTRIBUTIONLIKELIHOOD, CATCHSTATISTICSLIKELIHOOD, STOMACHCONTENTLIKELIHOOD,
  TAGLIKELIHOOD, STOCKDISTRIBUTIONLIKELIHOOD, PREDATORINDICESLIKELIHOOD,
  MIGRATIONPENALTYLIKELIHOOD, RANDOMWALKLIKELIHOOD, BOUNDLIKELIHOOD,
  LOGCATCHLIKELIHOOD, LOGSURVEYLIKELIHOOD, AGGREGATEDCDLIKELIHOOD, CATCHINTONSLIKELIHOOD };

/**
 * \class Likelihood
 * \brief This is the base class used to calculate the likelihood scores used to compare the modelled data in the input data
 * \note This will always be overridden by the derived classes that actually calculate the likelihood scores
 */
class Likelihood {
public:
  /**
   * \brief This is the default Likelihood constructor
   */
  Likelihood(LikelihoodType T, double w = 0) { likelihood = 0; weight = w; type = T; };
  /**
   * \brief This is the default Likelihood destructor
   */
  virtual ~Likelihood() {};
  virtual void AddToLikelihood(const TimeClass* const TimeInfo) = 0;
  virtual void AddToLikelihoodTimeAndKeeper(const TimeClass* const TimeInfo, Keeper* const Keeper) {};
  virtual void Reset(const Keeper* const keeper) { likelihood = 0; };
  virtual void Print(ofstream& outfile) const = 0;
  virtual void LikelihoodPrint(ofstream& outfile) const { outfile << "weight " << weight << endl; };
  /**
   * \brief This will return the weighted likelihood score for the likelihood component
   * \return weight*likelihood
   */
  double ReturnLikelihood() const { return weight * likelihood; };
  /**
   * \brief This will return the type of likelihood class
   * \return type
   */
  LikelihoodType Type() const { return type; };
  /**
   * \brief This will return the unweighted likelihood score for the likelihood component
   * \return likelihood
   */
  double UnweightedLikelihood() const { return likelihood; };
  /**
   * \brief This will return the weight applied to the likelihood component
   * \return type
   */
  double Weight() const { return weight; };
  virtual void CommandLinePrint(ofstream&, const TimeClass& time, const PrintInfo& print) {};
  virtual void SetFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {};
protected:
  /**
   * \brief This stores the calculated score for the likelihood component
   */
  double likelihood;
private:
  /**
   * \brief This stores the weight to be applied to the likelihood component
   */
  double weight;
  /**
   * \brief This denotes what type of likelihood class has been created
   */
  LikelihoodType type;
};

#endif
