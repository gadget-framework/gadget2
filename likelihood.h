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
   * \param T is the LikelihoodType for the likelihood component
   * \param w is the weight for the likelihood component
   */
  Likelihood(LikelihoodType T, double w = 0) { likelihood = 0; weight = w; type = T; };
  /**
   * \brief This is the default Likelihood destructor
   */
  virtual ~Likelihood() {};
  /**
   * \brief This function will calculate the likelihood score for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void AddToLikelihood(const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will calculate the likelihood score for the current model after adjusting the parameters
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  virtual void AddToLikelihoodTimeAndKeeper(const TimeClass* const TimeInfo, Keeper* const keeper) {};
  /**
   * \brief This function will reset the likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper) { likelihood = 0; };
  /**
   * \brief This function will print the summary likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const = 0;
  /**
   * \brief This function will print information from each likelihood calculation
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void LikelihoodPrint(ofstream& outfile) {};
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
   * \return weight
   */
  double Weight() const { return weight; };
  /**
   * \brief This function will print the information that is requested from the commandline for the current model
   * \param outfile is the ofstream that all the model information gets sent to
   * \param time is the TimeClass for the current model
   * \param print is the PrintInfo for the current model
   */
  virtual void CommandLinePrint(ofstream& outfile, const TimeClass& time, const PrintInfo& print) {};
  /**
   * \brief This will select the fleets and stocks required for to calculate the likelihood score
   * \param Fleets is the FleetPtrVector listing all the available fleets
   * \param Stocks is the StockPtrVector listing all the available stocks
   */
  virtual void SetFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {};
protected:
  /**
   * \brief This stores the calculated score for the likelihood component
   */
  double likelihood;
  /**
   * \brief This stores the weight to be applied to the likelihood component
   */
  double weight;
private:
  /**
   * \brief This denotes what type of likelihood class has been created
   */
  LikelihoodType type;
};

#endif
