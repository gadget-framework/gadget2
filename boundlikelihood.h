#ifndef boundlikelihood_h
#define boundlikelihood_h

#include "likelihood.h"
#include "commentstream.h"
#include "intvector.h"
#include "doublevector.h"
#include "errorhandler.h"
#include "parameter.h"

/**
 * \class BoundLikelihood
 * \brief This is the class used to calculate the likelihood scores based on the bounds of the parameters for the current model
 */
class BoundLikelihood : public Likelihood {
public:
  /**
   * \brief This is the default BoundLikelihood contructor
   * \param infile is the CommentStream to read the likelihood parameters from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param w is the weight for the likelihood component
   */
  BoundLikelihood(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, const Keeper* const keeper, double w);
  /**
   * \brief This is the default BoundLikelihood destructor
   */
  virtual ~BoundLikelihood() {};
  /**
   * \brief This function will reset the bounds information for the likelihood calculation
   * \param keeper is the Keeper for the current model
   */
  void Reset(const Keeper* const keeper);
  /**
   * \brief This function will calculate the likelihood score for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \note This function is not used for this likelihood component
   */
  virtual void AddToLikelihood(const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will print the summary likelihood information
   * \param outfile is the ofstream that all the model information gets sent to.
   * \note This function is not used for this likelihood component
   */
  virtual void Print(ofstream&) const {};
  /**
   * \brief This function will calculate the likelihood score for the current model after adjusting the parameters
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  virtual void AddToLikelihoodTimeAndKeeper(const TimeClass* const TimeInfo, Keeper* const keeper);
protected:
  IntVector switchnr;
  /**
   * \brief This is the vector of the upper bounds
   */
  DoubleVector upperbound;
  /**
   * \brief This is the vector of the lower bounds
   */
  DoubleVector lowerbound;
  /**
   * \brief This is the vector of the power applied when the bound has been exceeded
   */
  DoubleVector powers;
  /**
   * \brief This is the vector of the weights applied when the lower bound has been exceeded
   */
  DoubleVector lowerweights;
  /**
   * \brief This is the vector of the weights applied when the upper bound has been exceeded
   */
  DoubleVector upperweights;
  /**
   * \brief This is the vector of the penalty applied when the bound has been exceeded
   */
  DoubleVector likelihoods;
  /**
   * \brief This is the default weight applied when the lower bound is exceeded
   */
  double defLW;
  /**
   * \brief This is the default weight applied when the upper bound is exceeded
   */
  double defUW;
  /**
   * \brief This is the default power applied when the bound is exceeded
   */
  double defPower;
private:
  /**
   * \brief This is a flag used to check whether the vectors have been initialised
   */
  int checkInitialised;
};

#endif
