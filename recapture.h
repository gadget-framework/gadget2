#ifndef recapture_h
#define recapture_h

#include "likelihood.h"
#include "commentstream.h"
#include "recaggregator.h"
#include "doublematrixptrmatrix.h"
#include "tagptrvector.h"

/**
 * \class Recaptures
 * \brief This is the class used to calculate a likelihood score based on returns data from tagging experiments
 *
 * This class calculates a likelihood score based on the difference between recaptures data from tagging experiments within the model with recaptures data obtained from returns from tagging experiments.  The model will calculate the expected number of recaptures for the tagging experiments according to the model parameters, and aggregate this into specified length groups.  This recaptures data is then compared to the corresponding data obtained from the returns data from tagging experiments.
 */
class Recaptures : public Likelihood {
public:
  /**
   * \brief This is the Recaptures constructor
   * \param infile is the CommentStream to read the Recaptures data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param weight is the weight for the likelihood component
   * \param Tag is the TagPtrVector of tagging experiments for the current model
   * \param name is the name for the likelihood component
   */
  Recaptures(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double weight, TagPtrVector Tag, const char* name);
  /**
   * \brief This is the default Recaptures destructor
   */
  virtual ~Recaptures();
  /**
   * \brief This function will calculate the likelihood score for the Recaptures component
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void addLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This function will reset the Recaptures likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
  /**
   * \brief This function will print the summary Recaptures likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This will select the fleets and stocks required to calculate the Recaptures likelihood score
   * \param Fleets is the FleetPtrVector of all the available fleets
   * \param Stocks is the StockPtrVector of all the available stocks
   */
  virtual void setFleetsAndStocks(FleetPtrVector& Fleets, StockPtrVector& Stocks);
  /**
   * \brief This function will print summary information from the Recaptures likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void printSummary(ofstream& outfile);
  /**
   * \brief This function will print information from each Recaptures calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo);
private:
  /**
   * \brief This function will read the Recaptures data from the input file
   * \param infile is the CommentStream to read the Recaptures data from
   * \param TimeInfo is the TimeClass for the current model
   * \param numarea is the number of areas that the likelihood data covers
   * \param numlen is the number of length groups that the likelihood data covers
   */
  void readRecaptureData(CommentStream& infile,
    const TimeClass* const TimeInfo, int numarea, int numlen);
  /**
   * \brief This function will calculate the likelihood score for the current timestep based on a Poisson function
   * \param TimeInfo is the TimeClass for the current model
   * \return likelihood score
   */
  double calcLikPoisson(const TimeClass* const TimeInfo);
  /**
   * \brief This is the RecAggregator used to collect information about the racptured tagged fish
   */
  RecAggregator** aggregator;
  /**
   * \brief This is the CharPtrVector of the names of the tagging experiments that will be used to calculate the likelihood score
   */
  CharPtrVector tagnames;
  /**
   * \brief This is the CharPtrVector of the names of the fleets that will be used to calculate the likelihood score
   */
  CharPtrVector fleetnames;
  /**
   * \brief This is the CharPtrVector of the names of the areas
   */
  CharPtrVector areaindex;
  /**
   * \brief This is the CharPtrVector of the names of the length groups
   */
  CharPtrVector lenindex;
  /**
   * \brief This is the IntMatrix used to store area information
   */
  IntMatrix areas;
  /**
   * \brief This is the DoubleVector used to store length information
   */
  DoubleVector lengths;
  /**
   * \brief This is the IntMatrix used to store information about the years when recapture data is available
   */
  IntMatrix obsYears;
  /**
   * \brief This is the IntMatrix used to store information about the steps when recapture data is available
   */
  IntMatrix obsSteps;
  /**
   * \brief This is the IntMatrix used to store information about the years when recapture data is not available but modelled recaptures occur
   */
  IntMatrix modYears;
  /**
   * \brief This is the IntMatrix used to store information about the steps when recapture data is not available but modelled recaptures occur
   */
  IntMatrix modSteps;
  /**
   * \brief This is the DoubleMatrixPtrMatrix used to store length distribution information specified in the input file
   * \note The indices for this object are [tag][time][area][length]
   */
  DoubleMatrixPtrMatrix obsDistribution;
  /**
   * \brief This is the DoubleMatrixPtrMatrix used to store length distribution information calculated in the model for the timesteps that there were observed recaptures
   * \note The indices for this object are [tag][time][area][length]
   */
  DoubleMatrixPtrMatrix modelDistribution;
  /**
   * \brief This is the DoubleMatrixPtrMatrix used to store length distribution information calculated in the model for the timesteps that there were no observed recaptures, but there were still modelled recaptures
   * \note The indices for this object are [tag][time][area][length]
   */
  DoubleMatrixPtrMatrix newDistribution;
  /**
   * \brief This is the TagPtrVector of the relevant tagging experiments
   */
  TagPtrVector tagvec;
  /**
   * \brief This is the identifier of the function to be used to calculate the likelihood component
   */
  int functionnumber;
  /**
   * \brief This is the name of the function to be used to calculate the likelihood component
   */
  char* functionname;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to temporarily store the information returned from the aggregatation function
   */
  const AgeBandMatrixPtrVector* alptr;
};

#endif
