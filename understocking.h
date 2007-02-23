#ifndef understocking_h
#define understocking_h

#include "likelihood.h"
#include "areatime.h"
#include "commentstream.h"
#include "intmatrix.h"
#include "doublematrix.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"

/**
 * \class UnderStocking
 * \brief This is the class used to calculate a likelihood score based on the overconsumption of a stock by predators
 *
 * This class calculates a penalty that is applied if there are insufficient fish of a particular stock to meet the requirements for the predators - usually the fleet landings data.  This indicates that there are less fish in the model than have been landed (as recorded by the landings data), which is \b very wrong, and so this gets a high penalty.  A 'reasonable' model will have a zero likelihood score from this component.
 */
class UnderStocking : public Likelihood {
public:
  /**
   * \brief This is the UnderStocking constructor
   * \param infile is the CommentStream to read the understocking data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param weight is the weight for the likelihood component
   * \param name is the name for the likelihood component
   */
  UnderStocking(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double weight, const char* name);
  /**
   * \brief This is the default UnderStocking destructor
   */
  virtual ~UnderStocking();
  /**
   * \brief This function will calculate the likelihood score for the UnderStocking component
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void addLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This function will reset the UnderStocking likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
  /**
   * \brief This function will print summary information from each non-zero UnderStocking likelihood calculation
   * \param outfile is the ofstream that all the model likelihood information gets sent to
   */
  virtual void printSummary(ofstream& outfile);
  /**
   * \brief This function will print the non-zero UnderStocking likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This will select the predators and preys required to calculate the UnderStocking likelihood score
   * \param predvec is the PredatorPtrVector of all the available predators
   * \param preyvec is the PreyPtrVector of all the available preys
   * \param Area is the AreaClass for the current model
   */
  void setPredatorsAndPreys(PredatorPtrVector& predvec, PreyPtrVector& preyvec, const AreaClass* const Area);
private:
  /**
   * \brief This is the PredatorPtrVector of the predators that will be checked for understocking
   */
  PredatorPtrVector predators;
  /**
   * \brief This is the PreyPtrVector of the preys that will be checked for understocking
   */
  PreyPtrVector preys;
  /**
   * \brief This is the IntVector used to store area information
   */
  IntVector areas;
  /**
   * \brief This is the power coefficient used when calculating the likelihood score (default 2)
   */
  double powercoeff;
  /**
   * \brief This is the flag used to denote whether the likelihood score is to be based on the predation by all the predators or not
   */
  int allpredators;
  /**
   * \brief This is the IntVector used to store information about the years when the likelihood score was calculated to be non-zero
   */
  IntVector Years;
  /**
   * \brief This is the IntVector used to store information about the steps when the likelihood score was calculated to be non-zero
   */
  IntVector Steps;
  /**
   * \brief This is the DoubleVector used to store the calculated non-zero likelihood score
   */
  DoubleVector likelihoodValues;
  /**
   * \brief This is the CharPtrVector used to store the names of the predators that will be checked for understocking
   */
  CharPtrVector prednames;
};

#endif
