#ifndef understocking_h
#define understocking_h

#include "likelihood.h"
#include "actionattimes.h"
#include "commentstream.h"
#include "intmatrix.h"
#include "charptrvector.h"

/**
 * \class UnderStocking
 * \brief This is the class used to calculate a likelihood score based on the overconsumption of a stock by a fleet
 *
 * This class calculates a penalty that is applied if there are insufficient fish of a particular stock to meet the requirements set by the fleet landings data.  This indicates that there are less fish in the model than have been landed (as recorded by the landings data), which is <b>very</b> wrong, and so this gets a high penalty.  A 'reasonable' model will have a zero likelihood score from this component.
 */
class UnderStocking : public Likelihood {
public:
  /**
   * \brief This is the UnderStocking constructor
   * \param infile is the CommentStream to read the understocking data from
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param weight is the weight for the likelihood component
   */
  UnderStocking(CommentStream& infile, const AreaClass* const Area,
    const TimeClass* const TimeInfo, double weight);
  /**
   * \brief This is the default UnderStocking destructor
   */
  virtual ~UnderStocking();
  /**
   * \brief This function will calculate the likelihood score for the UnderStocking component
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void AddToLikelihood(const TimeClass* const TimeInfo);
  /**
   * \brief This function will reset the UnderStocking likelihood information
   * \param keeper is the Keeper for the current model
   */
  virtual void Reset(const Keeper* const keeper);
  /**
   * \brief This function will print the summary UnderStocking likelihood information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const {};
  /**
   * \brief This will select the fleets required to calculate the UnderStocking likelihood score
   * \param Fleets is the FleetPtrVector of all the available fleets
   */
  void setFleets(FleetPtrVector& Fleets);
private:
  /**
   * \brief This is the CharPtrVector of the names of the fleets that will be checked for understocking
   */
  CharPtrVector fleetnames;
  /**
   * \brief This is the FleetPtrVector of the fleets that will be checked for understocking
   */
  FleetPtrVector fleets;
  /**
   * \brief This is the IntMatrix used to store area information
   */
  IntMatrix areas;
  /**
   * \brief This is the CharPtrVector of the names of the areas
   */
  CharPtrVector areaindex;
  /**
   * \brief This ActionAtTimes stores information about when the likelihood score should be calculated
   */
  ActionAtTimes AAT;
  /**
   * \brief This is the power coefficient used when calculating the likelihood score (default 2)
   */
  double powercoeff;
};

#endif
