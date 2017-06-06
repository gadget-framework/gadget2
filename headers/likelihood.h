#ifndef likelihood_h
#define likelihood_h

#include "stockptrvector.h"
#include "fleetptrvector.h"
#include "printinfo.h"
#include "hasname.h"
#include "gadget.h"

class TimeClass;
class Keeper;

enum LikelihoodType { SURVEYINDICESLIKELIHOOD = 1, UNDERSTOCKINGLIKELIHOOD,
  CATCHDISTRIBUTIONLIKELIHOOD, CATCHSTATISTICSLIKELIHOOD, STOMACHCONTENTLIKELIHOOD,
  STOCKDISTRIBUTIONLIKELIHOOD, CATCHINKILOSLIKELIHOOD, BOUNDLIKELIHOOD, TAGLIKELIHOOD,
		      MIGRATIONPENALTYLIKELIHOOD, RECSTATISTICSLIKELIHOOD, SURVEYDISTRIBUTIONLIKELIHOOD, MIGRATIONPROPORTIONLIKELIHOOD ,PROGLIKELIHOOD};

/**
 * \class Likelihood
 * \brief This is the base class used to calculate the likelihood scores used to compare the modelled data in the input data
 * \note This will always be overridden by the derived classes that actually calculate the likelihood scores
 */
class Likelihood : public HasName {
public:
  /**
   * \brief This is the default Likelihood constructor
   * \param T is the LikelihoodType for the likelihood component
   * \param w is the weight for the likelihood component
   * \param givenname is the name of the likelihood component
   */
  Likelihood(LikelihoodType T, double w, const char* givenname) : HasName(givenname) {
    likelihood = 0.0;
    weight = w;
    type = T;
  };
  /**
   * \brief This is the default Likelihood destructor
   */
  virtual ~Likelihood() {};
  /**
   * \brief This function will calculate the likelihood score for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void addLikelihood(const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will calculate the likelihood score for the current model after adjusting the parameters
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  virtual void addLikelihoodKeeper(const TimeClass* const TimeInfo, Keeper* const keeper) {};
  /**
   * \brief This function will reset the likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper) { likelihood = 0.0; };
  /**
   * \brief This function will reset the likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will print the summary likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const = 0;
  /**
   * \brief This function will print information from each likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will print summary information from each likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void printSummary(ofstream& outfile) {};
  /**
   * \brief This will return the weighted likelihood score for the likelihood component
   * \return weight*likelihood
   */
  double getLikelihood() const { return weight * likelihood; };
  /**
   * \brief This will return the type of likelihood class
   * \return type
   */
  LikelihoodType getType() const { return type; };
  /**
   * \brief This will return the unweighted likelihood score for the likelihood component
   * \return likelihood
   */
  double getUnweightedLikelihood() const { return likelihood; };
  /**
   * \brief This will return the weight applied to the likelihood component
   * \return weight
   */
  double getWeight() const { return weight; };
  /**
   * \brief This will select the fleets and stocks required to calculate the likelihood score
   * \param Fleets is the FleetPtrVector of all the available fleets
   * \param Stocks is the StockPtrVector of all the available stocks
   */
  virtual void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks) {};
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
