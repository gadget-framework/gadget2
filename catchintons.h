#ifndef catchintons_h
#define catchintons_h

#include "likelihood.h"
#include "actionattimes.h"
#include "commentstream.h"
#include "charptrvector.h"
#include "intmatrix.h"

/**
 * \class CatchInTons
 * \brief This is the class used to calculate a likelihood score based on the biomass of stocks caught by fleets
 *
 * This class calculates a likelihood score based on the difference between the biomass caught according to the model and the biomass caught by fleets, according to the landings data.  This should lead to a zero likelihood score for a fleet that has been declared as a TotalPredator, since catch for that fleet should exactly match the landings data.  However, for a fleet that has been declared as either a LinearPredator or a MortPredator the model will be calculating the catch based on model parameters and so will probably lead to a difference between the modelled landings and actual landed catch.
 */
class CatchInTons : public Likelihood {
public:
  /**
   * \brief This is the CatchInTons constructor
   * \param infile is the CommentStream to read the CatchInTons data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param weight is the weight for the likelihood component
   * \param name is the name for the likelihood component
   */
  CatchInTons(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double weight, const char* name);
  /**
   * \brief This is the default CatchInTons destructor
   */
  ~CatchInTons();
  /**
   * \brief This function will reset the CatchInTons likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
  /**
   * \brief This function will print the summary CatchInTons likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This function will print summary information from each CatchInTons likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void SummaryPrint(ofstream& outfile);
  /**
   * \brief This function will print information from each CatchInTons likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void LikelihoodPrint(ofstream& outfile);
  /**
   * \brief This function will calculate the likelihood score for the CatchInTons component
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void addLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This will select the fleets and stocks required to calculate the CatchInTons likelihood score
   * \param Fleets is the FleetPtrVector of all the available fleets
   * \param Stocks is the StockPtrVector of all the available stocks
   */
  void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
private:
  /**
   * \brief This function will read the CatchInTons data from the input file
   * \param infile is the CommentStream to read the CatchInTons data from
   * \param TimeInfo is the TimeClass for the current model
   * \param numarea is the number of areas that the likelihood data covers
   */
  void readCatchInTonsData(CommentStream& infile, const TimeClass* TimeInfo, int numarea);
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a sum of squares function
   * \param TimeInfo is the TimeClass for the current model
   * \return likelihood score
   */
  double calcLikSumSquares(const TimeClass* const TimeInfo);
  /**
   * \brief This is the CharPtrVector of the names of the fleets that will be used to calculate the likelihood score
   */
  CharPtrVector fleetnames;
  /**
   * \brief This is the CharPtrVector of the names of the stocks that will be used to calculate the likelihood score
   */
  CharPtrVector stocknames;
  /**
   * \brief This is the CharPtrVector of the names of the areas
   */
  CharPtrVector areaindex;
  /**
   * \brief This is the IntMatrix used to denote which fleets catch which stocks
   */
  IntMatrix preyindex;
  /**
   * \brief This is the FleetPtrVector of the fleets that will be used to calculate the likelihood score
   */
  FleetPtrVector fleets;
  /**
   * \brief This is the StockPtrVector of the stocks that will be used to calculate the likelihood score
   */
  StockPtrVector stocks;
  /**
   * \brief This is the IntMatrix used to store area information
   */
  IntMatrix areas;
  /**
   * \brief This is the DoubleMatrix used to store catch information specified in the input file
   * \note the indices for this object are [time][area]
   */
  DoubleMatrix obsDistribution;
  /**
   * \brief This is the DoubleMatrix used to store catch information calculated in the model
   * \note the indices for this object are [time][area]
   */
  DoubleMatrix modelDistribution;
  /**
   * \brief This is the DoubleMatrix used to store the calculated likelihood information
   * \note the indices for this object are [time][area]
   */
  DoubleMatrix likelihoodValues;
  /**
   * \brief This is a flag to denote whether the likelihood calculation should aggregate data over a whole year.
   * \note the default value is 0, which calculates the likelihood score on each timestep
   */
  int yearly;
  /**
   * \brief This is the value of epsilon used when calculating the likelihood score
   */
  double epsilon;
  /**
   * \brief This ActionAtTimes stores information about when the likelihood score should be calculated
   */
  ActionAtTimes AAT;
  /**
   * \brief This is the IntVector used to store information about the years when the likelihood score should be calculated
   */
  IntVector Years;
  /**
   * \brief This is the IntVector used to store information about the steps when the likelihood score should be calculated
   */
  IntVector Steps;
  /**
   * \brief This is the index of the timesteps for the likelihood component data
   */
  int timeindex;
  /**
   * \brief This is the identifier of the function to be used to calculate the likelihood component
   */
  int functionnumber;
  /**
   * \brief This is the name of the function to be used to calculate the likelihood component
   */
  char* functionname;
  /**
   * \brief This is the name of the CatchInTons likelihood component
   */
  char* ctname;
};

#endif
