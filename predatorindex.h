#ifndef predatorindex_h
#define predatorindex_h

#include "commentstream.h"
#include "predatorptrvector.h"
#include "preyptrvector.h"
#include "lengthgroup.h"
#include "pionstep.h"

class PredatorIndices : public Likelihood {
public:
  /**
   * \brief This is the PredatorIndices constructor
   * \param infile is the CommentStream to read the PredatorIndices data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param weight is the weight for the likelihood component
   * \param name is the name for the likelihood component
   */
  PredatorIndices(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double weight, const char* name);
  /**
   * \brief This is the default PredatorIndices destructor
   */
  virtual ~PredatorIndices();
  /**
   * \brief This function will print the summary PredatorIndices likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This function will calculate the likelihood score for the PredatorIndices component
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void addLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This will select the predators and preys required to calculate the PredatorIndices likelihood score
   * \param Predators is the PredatorPtrVector of all the available predators
   * \param Preys is the PreyPtrVector of all the available preys
   */
  virtual void setPredatorsAndPreys(PredatorPtrVector& Predators, PreyPtrVector& Preys);
  /**
   * \brief This function will reset the PredatorIndices likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
protected:
  /**
   * \brief This is the CharPtrVector of the names of the predators that will be used to calculate the likelihood score
   */
  CharPtrVector predatornames;
  /**
   * \brief This is the CharPtrVector of the names of the preys that will be used to calculate the likelihood score
   */
  CharPtrVector preynames;
  PIOnStep* PI;
  /**
   * \brief This is the IntMatrix used to store area information
   */
  IntMatrix areas;
  /**
   * \brief This is the name of the PredatorIndices likelihood component
   */
  char* piname;
};

#endif
