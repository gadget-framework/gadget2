#ifndef logcatchfunction_h
#define logcatchfunction_h

#include "likelihood.h"
#include "commentstream.h"
#include "fleetpreyaggregator.h"
#include "doublematrixptrmatrix.h"
#include "actionattimes.h"

class LogCatches : public Likelihood {
public:
  /**
   * \brief This is the LogCatches constructor
   * \param infile is the CommentStream to read the LogCatches data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param weight is the weight for the likelihood component
   */
  LogCatches(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double weight);
  /**
   * \brief This is the default LogCatches destructor
   */
  virtual ~LogCatches();
  /**
   * \brief This function will calculate the likelihood score for the LogCatches component
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This function will reset the LogCatches likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
  /**
   * \brief This function will print the summary LogCatches likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const {};
  /**
   * \brief This will select the fleets and stocks required to calculate the LogCatches likelihood score
   * \param Fleets is the FleetPtrVector of all the available fleets
   * \param Stocks is the StockPtrVector of all the available stocks
   */
  void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
  /**
   * \brief This function will print information from each LogCatches likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void LikelihoodPrint(ofstream& outfile);
private:
  void readLogCatchData(CommentStream&, const TimeClass*, int, int, int);
  void readLogWeightsData(CommentStream&, const TimeClass*, int);
  double LogLik(const TimeClass* const TimeInfo);
  DoubleMatrixPtrMatrix AgeLengthData;
  DoubleMatrixPtrMatrix Proportions;
  DoubleMatrix Likelihoodvalues;
  FleetPreyAggregator* aggregator;
  LengthGroupDivision* lgrpDiv;
  CharPtrVector fleetnames;
  CharPtrVector stocknames;
  IntMatrix areas;
  IntMatrix ages;
  DoubleVector lengths;
  CharPtrVector areaindex;
  CharPtrVector ageindex;
  CharPtrVector lenindex;
  int overconsumption;    //should we take overconsumption into account
  int functionnumber;
  char* functionname;
  int timeindex;
  int readWeights;
  ActionAtTimes AAT;
  IntVector Years;
  IntVector Steps;
  DoubleMatrix weights;
  DoubleMatrixPtrVector calc_c;
  DoubleMatrixPtrVector obs_c;
  int agg_lev; //0: calculate likelihood on all time steps, or
               //1: calculate likelihood once a year
  int min_stock_age; //kgf 10/5 99
  int max_stock_age;//kgf 10/5 99
};

#endif
