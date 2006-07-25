#ifndef stomachcontent_h
#define stomachcontent_h

#include "commentstream.h"
#include "lengthgroup.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "doublematrixptrmatrix.h"
#include "intmatrix.h"
#include "formulamatrix.h"
#include "actionattimes.h"
#include "charptrmatrix.h"
#include "predatoraggregator.h"
#include "multinomial.h"

/**
 * \class SC
 * \brief This is the base class used to calculate the stomach content likelihood score based on the consumption of the preys in the model and the stomach content data specified in the input files
 * \note This will always be overridden by the derived classes that actually calculate the stomach content likelihood score
 */
class SC : public HasName {
public:
  /**
   * \brief This is the SC constructor
   * \param infile is the CommentStream to read the SC data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param datafilename is the name of the file containing the SC data
   * \param givenname is the name for the likelihood component
   */
  SC(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo,
    Keeper* const keeper, const char* datafilename, const char* givenname);
  /**
   * \brief This is the default SC destructor
   */
  virtual ~SC();
  /**
   * \brief This function will calculate the likelihood score from the SC information
   * \param TimeInfo is the TimeClass for the current model
   * \return likelihood score
   */
  virtual double calcLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This function will reset the SC information
   */
  virtual void Reset();
  /**
   * \brief This function will print the summary SC information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This will select the predators and preys required to calculate the SC likelihood score
   * \param Predators is the PredatorPtrVector of all the available predators
   * \param Preys is the PreyPtrVector of all the available preys
   */
  virtual void setPredatorsAndPreys(PredatorPtrVector& Predators, PreyPtrVector& Preys);
  /**
   * \brief This function will print summary information from each StomachContent likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   * \param weight is the weight for the likelihood component
   */
  virtual void printSummary(ofstream& outfile, double weight);
  /**
   * \brief This function will print information from each StomachContent calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo);
  /**
   * \brief This function will aggregate consumption information for each StomachContent calculation
   * \param i is the index of the prey that is being consumed
   */
  virtual void aggregate(int i);
protected:
  /**
   * \brief This function will calculate the likelihood score from the SC information
   * \return 0 (will be overridden in derived classes)
   */
  virtual double calcLikelihood() = 0;
  /**
   * \brief This is the DoubleMatrixPtrMatrix used to store stomach content (consumption) information specified in the input file
   * \note The indices for this object are [time][area][predator][prey]
   */
  DoubleMatrixPtrMatrix obsConsumption;
  /**
   * \brief This is the DoubleMatrixPtrMatrix used to store consumption (stomach content) information calculated in the model
   * \note The indices for this object are [time][area][predator][prey]
   */
  DoubleMatrixPtrMatrix modelConsumption;
  /**
   * \brief This is the DoubleMatrix used to store the calculated likelihood information
   * \note The indices for this object are [time][area]
   */
  DoubleMatrix likelihoodValues;
  /**
   * \brief This is the CharPtrVector of the names of the predators that will be used to calculate the likelihood score
   */
  CharPtrVector predatornames;
  /**
   * \brief This is the CharPtrMatrix of the names of the preys that will be used to calculate the likelihood score
   */
  CharPtrMatrix preynames;
  /**
   * \brief This is the CharPtrVector of the names of the areas
   */
  CharPtrVector areaindex;
  /**
   * \brief This is the CharPtrVector of the names of the predator aggregation units (either age or length) that will be read in from the input file
   */
  CharPtrVector predindex;
  /**
   * \brief This is the CharPtrVector of the names of the prey aggregation units that will be read in from the input file
   */
  CharPtrVector preyindex;
  /**
   * \brief This is the IntVector used to store information about the years when the likelihood score should be calculated
   */
  IntVector Years;
  /**
   * \brief This is the IntVector used to store information about the steps when the likelihood score should be calculated
   */
  IntVector Steps;
  /**
   * \brief This is the DoubleVector used to store predator length information
   */
  DoubleVector predatorlengths;
  /**
   * \brief This is the IntMatrix used to store predator age information
   */
  IntMatrix predatorages;
  /**
   * \brief This is the DoubleMatrix used to store prey length information
   * \note The indices for this object are [prey number][prey length]
   */
  DoubleMatrix preylengths;
  /**
   * \brief This is the IntMatrix used to store area information
   */
  IntMatrix areas;
  /**
   * \brief This is the flag used to denote whether the predators are specified using age groups or length groups
   */
  int usepredages;
  /**
   * \brief This is the PredatorAggregator used to collect information about the predation
   */
  PredatorAggregator** aggregator;
  /**
   * \brief This is the LengthGroupDivision used to store predator length information
   */
  LengthGroupDivision** preyLgrpDiv;
  /**
   * \brief This is the LengthGroupDivision used to store prey length information
   */
  LengthGroupDivision* predLgrpDiv;
  /**
   * \brief This is the index of the timesteps for the likelihood component data
   */
  int timeindex;
  /**
   * \brief This ActionAtTimes stores information about when the likelihood score should be calculated
   */
  ActionAtTimes AAT;
  /**
   * \brief This is the FormulaMatrix used to store the digestion parameters for the various predator prey pairs
   */
  FormulaMatrix digestioncoeff;
  /**
   * \brief This is the DoubleMatrix used to store the calculated digestion coefficiants for the various predator prey pairs
   */
  DoubleMatrix digestion;
  /**
   * \brief This is the value of epsilon used when calculating the likelihood score
   */
  double epsilon;
  /**
   * \brief This is the DoubleMatrix used to temporarily store the information returned from the aggregatation function
   */
  const DoubleMatrix* dptr;
};

/**
 * \class SCNumbers
 * \brief This is the class used to calculate the stomach content likelihood score based on a function of the numbers of prey consumed
 */
class SCNumbers : public SC {
public:
  /**
   * \brief This is the SCNumbers constructor
   * \param infile is the CommentStream to read the SCNumbers data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param datafilename is the name of the file containing the SCNumbers data
   * \param givenname is the name for the likelihood component
   */
  SCNumbers(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo,
    Keeper* const keeper, const char* datafilename, const char* givenname);
  /**
   * \brief This is the default SCNumbers destructor
   */
  virtual ~SCNumbers() {};
  /**
   * \brief This function will aggregate consumption information for each StomachContent calculation
   * \param i is the index of the prey that is being consumed
   */
  virtual void aggregate(int i);
protected:
  /**
   * \brief This function will read the StomachContent numbers data from the input file
   * \param infile is the CommentStream to read the StomachContent data from
   * \param TimeInfo is the TimeClass for the current model
   */
  void readStomachNumberContent(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the likelihood score from the SCNumbers information
   * \return likelihood score
   */
  virtual double calcLikelihood();
  /**
   * \brief This is the Multinomial that is used when calculating the likelihood score
   */
  Multinomial MN;
  /**
   * \brief This is the DoubleVector used to temporarily store the observed consumption when calculating the likelihood score
   */
  DoubleVector mndata;
  /**
   * \brief This is the DoubleVector used to temporarily store the modelled consumption when calculating the likelihood score
   */
  DoubleVector mndist;
};

/**
 * \class SCAmounts
 * \brief This is the class used to calculate the stomach content likelihood score based on a function of the amount of prey consumed, the standard deviation of this amount and the number of stomachs sampled
 */
class SCAmounts : public SC {
public:
  /**
   * \brief This is the SCAmounts constructor
   * \param infile is the CommentStream to read the SCAmounts data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param datafilename is the name of the file containing the SCAmounts data
   * \param numfilename is the name of the file containing the SCAmounts numbers data
   * \param givenname is the name for the likelihood component
   */
  SCAmounts(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper,
    const char* datafilename, const char* numfilename, const char* givenname);
  /**
   * \brief This is the default SCAmounts destructor
   */
  virtual ~SCAmounts();
  /**
   * \brief This function will print information from each SCAmounts calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo);
protected:
  /**
   * \brief This function will read the StomachContent amount data from the input file
   * \param infile is the CommentStream to read the StomachContent data from
   * \param TimeInfo is the TimeClass for the current model
   */
  void readStomachAmountContent(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This function will read the StomachContent sample data from the input file
   * \param infile is the CommentStream to read the StomachContent data from
   * \param TimeInfo is the TimeClass for the current model
   */
  void readStomachSampleContent(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the likelihood score from the SCAmounts information
   * \return likelihood score
   */
  virtual double calcLikelihood();
  /**
   * \brief This is the DoubleMatrixPtrMatrix used to store the standard deviation values for the consumption information specified in the input file
   * \note The indices for this object are [time][area][predator][prey]
   */
  DoubleMatrixPtrMatrix stddev;
  /**
   * \brief This is the DoubleMatrixPtrVector used to store the number of stomach samples specified in the input file
   * \note The indices for this object are [time][area][predator]
   */
  DoubleMatrixPtrVector number;
};

/**
 * \class SCRatios
 * \brief This is the class used to calculate the stomach content likelihood score based on a function of the ratios of the prey consumed, the standard deviation of this ratio and the number of stomachs sampled
 */
class SCRatios : public SCAmounts {
public:
  /**
   * \brief This is the SCRatios constructor
   * \param infile is the CommentStream to read the SCRatios data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param datafilename is the name of the file containing the SCRatios data
   * \param numfilename is the name of the file containing the SCRatios numbers data
   * \param givenname is the name for the likelihood component
   */
  SCRatios(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper,
    const char* datafilename, const char* numfilename, const char* givenname)
    : SCAmounts(infile, Area, TimeInfo, keeper, datafilename, numfilename, givenname) {};
  /**
   * \brief This is the default SCRatios destructor
   */
  virtual ~SCRatios() {};
  /**
   * \brief This will select the predators and preys required to calculate the SCRatios likelihood score
   * \param Predators is the PredatorPtrVector of all the available predators
   * \param Preys is the PreyPtrVector of all the available preys
   */
  virtual void setPredatorsAndPreys(PredatorPtrVector& Predators, PreyPtrVector& Preys);
protected:
  /**
   * \brief This function will calculate the likelihood score from the SCRatios information
   * \return likelihood score
   */
  virtual double calcLikelihood();
};

/**
 * \class SCSimple
 * \brief This is the class used to calculate the stomach content likelihood score based on a simple function of the ratios of the prey consumed
 */
class SCSimple : public SC {
public:
  /**
   * \brief This is the SCSimple constructor
   * \param infile is the CommentStream to read the SCNumbers data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param datafilename is the name of the file containing the SCSimple data
   * \param givenname is the name for the likelihood component
   */
  SCSimple(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo,
    Keeper* const keeper, const char* datafilename, const char* givenname);
  /**
   * \brief This is the default SCSimple destructor
   */
  virtual ~SCSimple() {};
  /**
   * \brief This will select the predators and preys required to calculate the SCSimple likelihood score
   * \param Predators is the PredatorPtrVector of all the available predators
   * \param Preys is the PreyPtrVector of all the available preys
   */
  virtual void setPredatorsAndPreys(PredatorPtrVector& Predators, PreyPtrVector& Preys);
protected:
  /**
   * \brief This function will read the StomachContent amount data from the input file
   * \param infile is the CommentStream to read the StomachContent data from
   * \param TimeInfo is the TimeClass for the current model
   */
  void readStomachSimpleContent(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the likelihood score from the SCSimple information
   * \return likelihood score
   */
  virtual double calcLikelihood();
};

/**
 * \class StomachContent
 * \brief This is the class used to calculate a likelihood score based on data sampled from the stomach of stocks caught by fleets
 *
 * This class calculates a likelihood score based on the difference between stomach contents data sampled from stocks caught according to the model and that caught by fleets, according to the landings data.  The stomach contents data can either be aggregated into length groups (giving a distribution of prey groups for each length) or into prey groups (giving a distribution of length groups for each prey).  The model will calculate the stomach contents data for the predators that are caught according to the model parameters, and then compare this to the corresponding data calculated from the observed stomach contents data
 *
 * \note Care is needed when making this comparison, since the data will give information on the stomach content at the time of capture of the predator, where as the model can only give information about the modelled consumption of the prey by the predator.
 */
class StomachContent : public Likelihood {
public:
  /**
   * \brief This is the StomachContent constructor
   * \param infile is the CommentStream to read the StomachContent data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param weight is the weight for the likelihood component
   * \param name is the name for the likelihood component
   */
  StomachContent(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper, double weight, const char* name);
  /**
   * \brief This is the default StomachContent destructor
   */
  virtual ~StomachContent();
  /**
   * \brief This function will calculate the likelihood score for the StomachContent component
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void addLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This function will reset the StomachContent likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
  /**
   * \brief This function will print the summary StomachContent likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This will select the predators and preys required to calculate the StomachContent likelihood score
   * \param Predators is the PredatorPtrVector of all the available predators
   * \param Preys is the PreyPtrVector of all the available preys
   */
  void setPredatorsAndPreys(PredatorPtrVector& Predators, PreyPtrVector& Preys)
    { StomCont->setPredatorsAndPreys(Predators, Preys); };
  /**
   * \brief This function will print summary information from each StomachContent likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void printSummary(ofstream& outfile) {
    StomCont->printSummary(outfile, weight); };
  /**
   * \brief This function will print information from each StomachContent calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void printLikelihood(ofstream& outfile, const TimeClass* const TimeInfo) {
    StomCont->printLikelihood(outfile, TimeInfo); };
private:
  /**
   * \brief This is the name of the function to be used to calculate the likelihood component
   */
  char* functionname;
  /**
   * \brief This is the SC used to calculate the likelihood score by calulating the consumption from the modelled data and comparing it to the stomach contents data specified in the input file
   */
  SC* StomCont;
};

#endif

