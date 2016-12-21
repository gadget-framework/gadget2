#ifndef actionattimes_h
#define actionattimes_h

#include "commentstream.h"
#include "areatime.h"
#include "intvector.h"

/**
 * \class ActionAtTimes
 * \brief This is the class used to associate actions with specific timesteps within the model
 *
 * This class is used to determine whether an action should take place on the current timestep in the model.  The actions include changing the population of the stock (either by fishing from the stock or adding to the stock) calculating the score from a likelihood component or printing output from the model.  This is done by storing a list of timesteps when the action takes place, and comparing it to the current timestep in the model.
 */
class ActionAtTimes {
public:
  /**
   * \brief This is the ActionAtTimes constructor
   */
  ActionAtTimes() { everyStep = 0; };
  /**
   * \brief This is the ActionAtTimes destructor
   */
  ~ActionAtTimes() {};
  /**
   * \brief This is the function used to read in the action at times parameters
   * \param infile is the CommentStream to read the actionattimes parameters from
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the read from file was successful, 0 otherwise
   */
  int readFromFile(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This is the function used to add the action for a set of steps and years
   * \param steps is the IntVector of all the steps that have the action taking place
   * \param years is the IntVector of all the years that have the action taking place
   * \param TimeInfo is the TimeClass for the current model
   */
  void addActions(const IntVector& years, const IntVector& steps, const TimeClass* const TimeInfo);
  /**
   * \brief This is the function used to add the action on all years for a set of steps
   * \param steps is the IntVector of all the steps that have the action taking place
   * \param TimeInfo is the TimeClass for the current model
   */
  void addActionsAllYears(const IntVector& steps, const TimeClass* const TimeInfo);
  /**
   * \brief This is the function used to add the action on all steps for a set of years
   * \param years is the IntVector of all the years that have the action taking place
   * \param TimeInfo is the TimeClass for the current model
   */
  void addActionsAllSteps(const IntVector& years, const TimeClass* const TimeInfo);
  /**
   * \brief This is the function used to check whether an action takes place on the current timestep
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if action takes place, 0 otherwise
   */
  int atCurrentTime(const TimeClass* const TimeInfo) const;
protected:
  /**
   * \brief This is the flag if the action takes place on every timestep
   */
  int everyStep;
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
