#ifndef surveyindices_h
#define surveyindices_h

#include "likelihood.h"
#include "fleet.h"
#include "commentstream.h"
#include "sionstep.h"

/**
 * \class SurveyIndices
 * \brief This is the class used to calculate a likelihood score by fitting a linear regression line to calculated and modelled survey indices
 *
 * This class calculates a likelihood score by fitting a linear regression line to the difference between survey indices calculated within the model and survey indices calculated directly from standardized survey data.  This is used to compare the relative abundance levels of the stock.  The model will calculate an index for the population, and then fit a linear regression line between these calculated indices and those input from data files.  The precise format of the linear regression line depends on a number of options specified by the user.
 *
 * \note This likelihood component is based on the \b current population, where as most likelihood components are based on the population at the beginning of the timestep.  It would be better if this likelihood component was changed to be based on the population at the beginning of the timestep.
 */
class SurveyIndices : public Likelihood {
public:
  /**
   * \brief This is the SurveyIndices constructor
   * \param infile is the CommentStream to read the SurveyIndices data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param weight is the weight for the likelihood component
   * \param name is the name for the likelihood component
   */
  SurveyIndices(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double weight, const char* name);
  /**
   * \brief This is the default SurveyIndices destructor
   */
  virtual ~SurveyIndices();
  /**
   * \brief This function will calculate the likelihood score for the SurveyIndices component
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void addLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This will select the fleets and stocks required to calculate the SurveyIndices likelihood score
   * \param Fleets is the FleetPtrVector of all the available fleets
   * \param Stocks is the StockPtrVector of all the available stocks
   */
  void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
  /**
   * \brief This function will reset the SurveyIndices likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
  /**
   * \brief This function will print the summary SurveyIndices likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This function will print information from each SurveyIndices calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo) {
    SI->printLikelihood(outfile, TimeInfo); };
  /**
   * \brief This function will print summary information from each SurveyIndices likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void printSummary(ofstream& outfile) { SI->printSummary(outfile, weight); };
protected:
  /**
   * \brief This is the SIOnStep used to calculate the likelihood score by fitting a linear regression line to the difference between the calculated indices and the input indices
   */
  SIOnStep* SI;
  /**
   * \brief This is the IntMatrix used to store area information
   */
  IntMatrix areas;
  /**
   * \brief This is the CharPtrVector of the names of the stocks that will be used to calculate the likelihood score
   */
  CharPtrVector stocknames;
  /**
   * \brief This is the CharPtrVector of the names of any fleets that will be used to calculate the likelihood score
   */
  CharPtrVector fleetnames;
};

#endif
