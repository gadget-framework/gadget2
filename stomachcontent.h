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

class SC {
public:
  /**
   * \brief This is the SC constructor
   * \param infile is the CommentStream to read the SC data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param datafilename is the name of the file containing the SC data
   * \param name is the name for the likelihood component
   */
  SC(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo,
    Keeper* const keeper, const char* datafilename, const char* name);
  /**
   * \brief This is the default SC destructor
   */
  virtual ~SC();
  virtual double Likelihood(const TimeClass* const TimeInfo);
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
  virtual void Aggregate(int i);
  virtual void PrintLikelihood(ofstream& outfile, const TimeClass& TimeInfo) {};
  virtual void PrintLikelihoodHeader(ofstream& outfile) {};
protected:
  virtual double CalculateLikelihood(DoubleMatrixPtrVector& consumption, DoubleMatrix& sum) = 0;
  DoubleMatrixPtrMatrix stomachcontent;   //[timeindex][areas][pred_l][prey_l]
  DoubleMatrixPtrMatrix modelConsumption; //[timeindex][areas][pred_l][prey_l]
  CharPtrVector predatornames;
  CharPtrMatrix preynames;
  CharPtrVector areaindex;
  CharPtrVector predindex;
  CharPtrVector preyindex;
  IntVector Years;
  IntVector Steps;
  DoubleVector predatorlengths;
  IntVector predatorages; //as an alternative to predatorlengths kgf 19/2 99
  DoubleMatrix preylengths;              //[prey_nr][prey_l]
  IntMatrix areas;
  int age_pred; //kgf 22/2 99 to switch between age or length predator
  PredatorAggregator** aggregator;
  LengthGroupDivision** preyLgrpDiv;
  LengthGroupDivision** predLgrpDiv;
  int timeindex;
  ActionAtTimes AAT;
  FormulaMatrix digestioncoeff;
  double epsilon;
  char* scname;
};

class SCNumbers : public SC {
public:
  /**
   * \brief This is the SCNumbers constructor
   * \param infile is the CommentStream to read the SCNumbers data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param datafilename is the name of the file containing the SCNumbers data
   * \param numfilename is the name of the file containing the SCNumbers numbers data (NULL)
   * \param name is the name for the likelihood component
   */
  SCNumbers(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper,
    const char* datafilename, const char* numfilename, const char* name);
  /**
   * \brief This is the default SCNumbers destructor
   */
  virtual ~SCNumbers() {};
  virtual void Aggregate(int i);
protected:
  /**
   * \brief This function will read the StomachContent numbers data from the input file
   * \param infile is the CommentStream to read the StomachContent data from
   * \param TimeInfo is the TimeClass for the current model
   */
  void readStomachNumberContent(CommentStream& infile, const TimeClass* const TimeInfo);
  virtual double CalculateLikelihood(DoubleMatrixPtrVector& consumption, DoubleMatrix&);
};

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
   * \param name is the name for the likelihood component
   */
  SCAmounts(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper,
    const char* datafilename, const char* numfilename, const char* name);
  /**
   * \brief This is the default SCAmounts destructor
   */
  virtual ~SCAmounts();
  virtual void PrintLikelihood(ofstream& outfile, const TimeClass& TimeInfo);
  virtual void PrintLikelihoodHeader(ofstream& outfile);
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
  virtual double CalculateLikelihood(DoubleMatrixPtrVector& consumption, DoubleMatrix& sum);
  DoubleMatrixPtrMatrix stddev;  //[timeindex][areas][pred_l][prey_l]
  DoubleMatrixPtrVector number;  //[timeindex][areas][pred_l]
};

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
   * \param name is the name for the likelihood component
   */
  SCRatios(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper,
    const char* datafilename, const char* numfilename, const char* name)
    : SCAmounts(infile, Area, TimeInfo, keeper, datafilename, numfilename, name) {};
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
  virtual double CalculateLikelihood(DoubleMatrixPtrVector& consumption, DoubleMatrix& sum);
};

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
  virtual void Reset(const Keeper* const keeper)
    { Likelihood::Reset(keeper); StomCont->Reset(); };
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
  virtual void PrintLikelihood(ofstream& outfile, const TimeClass& time)
    { StomCont->PrintLikelihood(outfile, time); };
  virtual void PrintLikelihoodHeader(ofstream& outfile)
    { StomCont->PrintLikelihoodHeader(outfile); };
private:
  /**
   * \brief This is the identifier of the function to be used to calculate the likelihood component
   */
  int functionnumber;
  /**
   * \brief This is the name of the function to be used to calculate the likelihood component
   */
  char* functionname;
  /**
   * \brief This is the name of the StomachContent likelihood component
   */
  char* stomachname;
  SC* StomCont;
};

#endif

