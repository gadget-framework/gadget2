#ifndef actionattimes_h
#define actionattimes_h

#include "commentstream.h"
#include "areatime.h"
#include "intvector.h"

class ActionAtTimes;

/**
 * \class ActionAtTimes
 * \brief This is the class used to associate actions with specific timesteps within the model
 */
class ActionAtTimes {
public:
  /**
   * \brief This is the ActionAtTimes constructor
   */
  ActionAtTimes();
  /**
   * \brief This is the ActionAtTimes destructor
   */
  ~ActionAtTimes();
  /**
   * \brief This is the function used to read in the action at times parameters
   * \param infile is the CommentStream to read the actionattimes parameters from
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 for success, 0 for failure
   */
  int readFromFile(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This is the function used to add the action for a set of steps and years
   * \param steps is the IntVector of all the steps that have the action taking place
   * \param years is the IntVector of all the years that have the action taking place
   * \param TimeInfo is the TimeClass for the current model
   */
  void AddActions(const IntVector& years, const IntVector& steps, const TimeClass* const TimeInfo);
  /**
   * \brief This is the function used to add the action on all years for a set of steps
   * \param steps is the IntVector of all the steps that have the action taking place
   * \param TimeInfo is the TimeClass for the current model
   */
  void AddActionsAtAllYears(const IntVector& steps, const TimeClass* const TimeInfo);
  /**
   * \brief This is the function used to add the action on all steps for a set of years
   * \param years is the IntVector of all the years that have the action taking place
   * \param TimeInfo is the TimeClass for the current model
   */
  void AddActionsAtAllSteps(const IntVector& years, const TimeClass* const TimeInfo);
  /**
   * \brief This is the function used to read check whether an action takes place on the current timestep
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if action takes place, 0 otherwise
   */
  int AtCurrentTime(const TimeClass* const TimeInfo) const;
protected:
  /**
   * \brief This is a flag if the action takes place on every timestep
   */
  int EveryStep;
  /**
   * \brief This is the IntVector of the timesteps on which the action takes place
   */
  IntVector TimeSteps;
  /**
   * \brief This is the IntVector of the years on which the action takes place
   */
  IntVector Years;
  /**
   * \brief This is the IntVector of the steps on which the action takes place
   */
  IntVector Steps;
};

#endif
