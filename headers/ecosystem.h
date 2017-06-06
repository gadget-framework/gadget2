#ifndef ecosystem_h
#define ecosystem_h

#include "areatime.h"
#include "keeper.h"
#include "errorhandler.h"
#include "tagptrvector.h"
#include "baseclassptrvector.h"
#include "stockptrvector.h"
#include "fleetptrvector.h"
#include "otherfoodptrvector.h"
#include "printerptrvector.h"
#include "likelihoodptrvector.h"
#include "optinfoptrvector.h"
#include "stock.h"
#include "fleet.h"
#include "otherfood.h"
#include "printer.h"
#include "maininfo.h"
#include "printinfo.h"
#include "optinfo.h"
#include "gadget.h"

/**
 * \class Ecosystem
 * \brief This is the class used to control the model simulation
 *
 * This is the main class for the model simulation.  This class contains pointers to all the objects that are to be used in the model simulation - the stocks and fleets, the model variables, any printer classes used to output the modelled population and any likelihood classes used to compare the modelled population to data.  This class controls the model simulation, from the reading of the input files, simulating the modelled population on each timestep and printing the results, and finally calculating an overall likelihood score.  This class is also used during an optimising run to update the variables to new values from the optimisation algorithm and calculating a new likelihood score based on these new values.
 */
class Ecosystem {
public:
  /**
   * \brief This is the default Ecosystem constructor
   */
  Ecosystem() {};
  /**
   * \brief This is the Ecosystem constructor specifying details about the model
   * \param main is the MainInfo specifying the command line options for the model run
   */
  Ecosystem(const MainInfo& main);
  /**
   * \brief This is the default Ecosystem destructor
   */
  ~Ecosystem();
  /**
   * \brief This function will read the model data from the main input file
   * \param infile is the CommentStream to read the likelihood data from
   * \param main is the MainInfo specifying the command line options for the model run
   */
  void readMain(CommentStream& infile, const MainInfo& main);
  /**
   * \brief This function will read the likelihood data from the input file
   * \param infile is the CommentStream to read the likelihood data from
   */
  void readLikelihood(CommentStream& infile);
  /**
   * \brief This function will read the printer data from the input file
   * \param infile is the CommentStream to read the printer data from
   */
  void readPrinters(CommentStream& infile);
  /**
   * \brief This function will read the fleet data from the input file
   * \param infile is the CommentStream to read the fleet data from
   */
  void readFleet(CommentStream& infile);
  /**
   * \brief This function will read the tagging data from the input file
   * \param infile is the CommentStream to read the tagging data from
   */
  void readTagging(CommentStream& infile);
  /**
   * \brief This function will read the otherfood data from the input file
   * \param infile is the CommentStream to read the otherfood data from
   */
  void readOtherFood(CommentStream& infile);
  /**
   * \brief This function will read the stock data from the input file
   * \param infile is the CommentStream to read the stock data from
   */
  void readStock(CommentStream& infile);
  /**
   * \brief This function will read in the optimisation parameters from the input file
   * \param infile is the CommentStream to read the optimisation parameters from
   */
  void readOptimisation(CommentStream& infile);
  /**
   * \brief This function will write the current model status to file
   * \param filename is the name of the file to write the model information to
   */
  void writeStatus(const char* filename) const;
  /**
   * \brief This function will write header information about the model parameters to file
   * \param filename is the name of the file to write the model information to
   */
  void writeInitialInformation(const char* const filename);
  /**
   * \brief This function will write current information about the model parameters to file
   */
  void writeValues();
  /**
   * \brief This function will write final information about the model parameters to file in a column format (which can then be used as the starting point for a subsequent model run)
   * \param filename is the name of the file to write the model information to
   * \param prec is the precision to use in the output file
   */
  void writeParams(const char* const filename, int prec) const;
  /**
   * \brief This function will display information about the best values of the parameters found so far during an optimisation run
   */
  void writeBestValues() { keeper->writeBestValues(); };
  /**
   * \brief This function will display information about the optimised values of the parameters
   */
  void writeOptValues();
  /**
   * \brief This is the function that will optimise the likelihood score
   */
  void Optimise();
  /**
   * \brief This function will initialise the Ecosystem parameters
   */
  void Initialise();
  /**
   * \brief This function will update the Ecosystem parameters with new values from StochasticData
   * \param Stochastic is the StochasticData containing the new values of the parameters
   */
  void Update(const StochasticData* const Stochastic) const {keeper->Update(Stochastic); };
  /**
   * \brief This function will reset the Ecosystem information
   */
  void Reset();
  /**
   * \brief This function will store the current value of the variables from the optimisation process
   * \param likvalue is the current likelihood value
   * \param point is the DoubleVector of the current parameter values
   */
  void storeVariables(double likvalue, const DoubleVector& point) { keeper->storeVariables(likvalue, point); };
  /**
   * \brief This function will reset the variables that have been scaled, to be optimised using the Simulated Annealing optimisation algorithm
   */
  void resetVariables() { keeper->resetVariables(); };
  /**
   * \brief This function will scale the variables to be optimised, for the Hooke & Jeeves and BFGS optimisation algorithms
   */
  void scaleVariables() { keeper->scaleVariables(); };
  /**
   * \brief This function will return a copy of the initial value of the variables to be optimised
   * \param val is the DoubleVector that will contain a copy of the initial values
   */
  void getOptInitialValues(DoubleVector& val) const { keeper->getOptInitialValues(val); };
  /**
   * \brief This function will return a copy of the scaled value of the variables to be optimised
   * \param val is the DoubleVector that will contain a copy of the scaled values
   */
  void getOptScaledValues(DoubleVector& val) const { keeper->getOptScaledValues(val); };
  /**
   * \brief This function will return a copy of the lower bounds of the variables to be optimised
   * \param lbs is the DoubleVector that will contain a copy of the lower bounds
   */
  void getOptLowerBounds(DoubleVector& lbs) const { keeper->getOptLowerBounds(lbs); };
  /**
   * \brief This function will return a copy of the upper bounds of the variables to be optimised
   * \param ubs is the DoubleVector that will contain a copy of the upper bounds
   */
  void getOptUpperBounds(DoubleVector& ubs) const { keeper->getOptUpperBounds(ubs); };
  /**
   * \brief This function will check that the values of the parameters are within the specified bounds
   */
  void checkBounds() const { keeper->checkBounds(likevec); };
  /**
   * \brief This function will return the number of variables
   * \return number of variables
   */
  int numVariables() const { return keeper->numVariables(); };
  /**
   * \brief This function will return the number of variables to be optimised
   * \return number of variables to be optimised
   */
  int numOptVariables() const { return keeper->numOptVariables(); };
  /**
   * \brief This function will simulate the population in the model
   * \param print is the flag to denote whether the model output should be printed or not
   * \note This function covers a single running of the model, calculating the population structure and also calculating the likelihood score obtained from comparing the modelled population to the data specified in the likelihood components
   */
  void Simulate(int print);
  /**
   * \brief This function will calculate the predation, and update the population, on an area
   * \param area is the area to calculate the predation on (default value 0)
   * \note This function covers the predation of the preys by the predators, and consequent population changes, for a sub-step within a timestep of the model
   */
  void updatePredationOneArea(int area = 0);
  /**
   * \brief This function will update the population numbers on an area
   * \param area is the area to update the population on (default value 0)
   * \note This function covers the growth, various movements between stocks due to maturity and straying, spawning, and adding new recruits into the model
   */
  void updatePopulationOneArea(int area = 0);
  /**
   * \brief This function will update the ages of the population on an area
   * \param area is the area to update the population on (default value 0)
   * \note This function covers the increase in age and the simple 'doesmove' option for movement between stocks
   */
  void updateAgesOneArea(int area = 0);
  /**
   * \brief This function will update the model parameters, run the model and calculate a likelihood score
   * \param x is the DoubleVector containing the updated values for the parameters
   * \return likelihood score
   * \note This function gets updated values for the model parameters from the optimisation algorithm and performs a model run based on these new values, calculating a new likelihood score that is then returned to the optimisation algorithm
   */
  double SimulateAndUpdate(const DoubleVector& x);
  /**
   * \brief This function will return the likelihood score from the current simulation
   * \return likelihood score
   */
  double getLikelihood() const { return likelihood; };
  /**
   * \brief This function will return the total number of iterations completed
   * \return number of iterations
   */
  int getFuncEval() const { return funceval; };
  /**
   * \brief This function will return the current time in the simulation
   * \return current time
   */
  int getCurrentTime() const { return TimeInfo->getTime(); };
  /**
   * \brief This function will return the current year in the simulation
   * \return current year
   */
  int getCurrentYear() const { return TimeInfo->getYear(); };
  /**
   * \brief This function will return the current step in the simulation
   * \return current step
   */
  int getCurrentStep() const { return TimeInfo->getStep(); };
  /**
   * \brief This function will return the total number of timestep in the simulation
   * \return number of timesteps
   */
  int numTotalSteps() const { return TimeInfo->numTotalSteps(); };
  /**
   * \brief This function will return the likelihood components used in the simulation
   * \return LikelihoodPtrVector containing all the likelihood components used in the simulation
   */
  LikelihoodPtrVector& getModelLikelihoodVector() { return likevec; };
  /**
   * \brief This function will return the printer components used in the simulation
   * \return PrinterPtrVector containing all the printer components used in the simulation
   */
  PrinterPtrVector& getModelPrinterVector() { return printvec; };
  /**
   * \brief This function will return the fleets used in the simulation
   * \return FleetPtrVector containing all the fleets used in the simulation
   */
  FleetPtrVector& getModelFleetVector() { return fleetvec; };
  /**
   * \brief This function will return the stocks used in the simulation
   * \return StockPtrVector containing all the stocks used in the simulation
   */
  StockPtrVector& getModelStockVector() { return stockvec; };
  /**
   * \brief This is the flag used to denote whether the user has interrupted the current model run
   */
  volatile int interrupted;
protected:
  /**
   * \brief This is the value of the likelihood score for the current simulation
   */
  double likelihood;
  /**
   * \brief This is the current iteration
   */
  int funceval;
  /**
   * \brief This is the BaseClassPtrVector of the stocks, fleets and otherfood classes for the current model
   */
  BaseClassPtrVector basevec;
  /**
   * \brief This is the LikelihoodPtrVector of the likelihood components for the current model
   */
  LikelihoodPtrVector likevec;
  /**
   * \brief This is the PrinterPtrVector of the printer classes for the current model
   */
  PrinterPtrVector printvec;
  /**
   * \brief This is the OptInfoPtrVector of the optimisation algorithms for the current model
   */
  OptInfoPtrVector optvec;
  /**
   * \brief This is the TimeClass for the current model
   */
  TimeClass* TimeInfo;
  /**
   * \brief This is the AreaClass for the current model
   */
  AreaClass* Area;
  /**
   * \brief This is the Keeper for the current model
   */
  Keeper* keeper;
  /**
   * \brief This is the StockPtrVector of the stocks for the current model
   */
  StockPtrVector stockvec;
  /**
   * \brief This is the TagPtrVector of the tagging experiments for the current model
   */
  TagPtrVector tagvec;
  /**
   * \brief This is the OtherFoodPtrVector of the otherfood for the current model
   */
  OtherFoodPtrVector otherfoodvec;
  /**
   * \brief This is the FleetPtrVector of the fleets for the current model
   */
  FleetPtrVector fleetvec;
  /**
   * \brief This is the PrintInfo specifying the parameter output from the current model
   */
  PrintInfo printinfo;
  /**
   * \brief This is the counter for the printing interval for the -o output from the simualtion
   */
  int printcount;
  /**
   * \brief This is the DoubleVector used to store the initial values of the parameters
   * \note This vector is only used to temporarily store values during an optimising run
   */
  DoubleVector initialval;
  /**
   * \brief This is the DoubleVector used to store the current values of the parameters
   * \note This vector is only used to temporarily store values during an optimising run
   */
  DoubleVector currentval;
  /**
   * \brief This is the IntVector used to store information about whether the parameters are to be optimised
   * \note This vector is only used to temporarily store values during an optimising run
   */
  IntVector optflag;
};

#endif
