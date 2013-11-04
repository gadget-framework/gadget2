#ifndef migrationproportion_h
#define migrationproportion_h

#include "likelihood.h"
#include "intmatrix.h"
#include "doublematrix.h"
#include "actionattimes.h"
#include "charptrvector.h"
#include "stockptrvector.h"

/**
 * \class MigrationProportion
 * \brief This is the class used to calculate a likelihood score based on the biomass of stocks caught by fleets
 *
 * This class calculates a likelihood score based on the difference between the biomass caught according to the model and the biomass caught by fleets, according to the landings data.  This should lead to a zero likelihood score for a fleet that has been declared as a TotalPredator, since catch for that fleet should exactly match the landings data.  However, for a fleet that has been declared as a LinearPredator, or a NumberPredator, the model will be calculating the catch based on model parameters and so will probably lead to a difference between the modelled landings biomass and actual landed catch.
 */
class MigrationProportion : public Likelihood {
public:
  /**
   * \brief This is the MigrationProportion constructor
   * \param infile is the CommentStream to read the CatchDistribution data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param weight is the weight for the likelihood component
   * \param name is the name for the likelihood component
   */
  MigrationProportion(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double weight, const char* name);
  /**
   * \brief This is the default MigrationProportion destructor
   */
  virtual ~MigrationProportion();
  /**
   * \brief This function will calculate the likelihood score for the MigrationProportion component
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void addLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This function will reset the MigrationProportion likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
  /**
   * \brief This function will print the summary MigrationProportion likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This will select the fleets and stocks required to calculate the MigrationProportion likelihood score
   * \param Fleets is the FleetPtrVector of all the available fleets
   * \param Stocks is the StockPtrVector of all the available stocks
   */
  void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
  /**
   * \brief This function will print information from each MigrationProportion calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo);
  /**
   * \brief This function will print summary information from each MigrationProportion likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void printSummary(ofstream& outfile);
private:
  /**
   * \brief This function will read the MigrationProportion data from the input file
   * \param infile is the CommentStream to read the CatchDistribution data from
   * \param TimeInfo is the TimeClass for the current model
   * \param numarea is the number of areas that the likelihood data covers
   */
  void readProportionData(CommentStream& infile, const TimeClass* TimeInfo, int numarea);
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a sum of squares function
   * \param TimeInfo is the TimeClass for the current model
   * \return likelihood score
   */
  double calcLikSumSquares(const TimeClass* const TimeInfo);
  /**
   * \brief This is the DoubleMatrix used to store area distribution information specified in the input file
   * \note The indices for this object are [time][area]
   */
  DoubleMatrix obsDistribution;
  /**
   * \brief This is the DoubleMatrix used to store area distribution information calculated in the model
   * \note The indices for this object are [time][area]
   */
  DoubleMatrix modelDistribution;
  /**
   * \brief This is the DoubleVector used to store the calculated likelihood information
   * \note The indices for this object are [time]
   */
  DoubleVector likelihoodValues;
  /**
   * \brief This is the CharPtrVector of the names of the stocks that will be used to calculate the likelihood score
   */
  CharPtrVector stocknames;
  /**
   * \brief This is the IntMatrix used to store area information
   */
  IntMatrix areas;
  /**
   * \brief This is the CharPtrVector of the names of the areas
   */
  CharPtrVector areaindex;
  /**
   * \brief This is the identifier of the function to be used to calculate the likelihood component
   */
  int functionnumber;
  /**
   * \brief This is the name of the function to be used to calculate the likelihood component
   */
  char* functionname;
  /**
   * \brief This is the flag used to denote whether the biomass consumed or the numbers consumed is used to calculate the likelihood score (default is to print biomass)
   */
  int biomass;
  /**
   * \brief This is the index of the timesteps for the likelihood component data
   */
  int timeindex;
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
   * \brief This is the StockPtrVector of the stocks that will be used to calculate the likelihood score
   */
  StockPtrVector stocks;
};

#endif
