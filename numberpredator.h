#ifndef numberpredator_h
#define numberpredator_h

#include "lengthpredator.h"

/**
 * \class NumberPredator
 * \brief This is the class used to model the predation of stocks by a fleet that will catch a specified number of preys, calculating the catch by numbers, and not by biomass, of the prey available
 */
class NumberPredator : public LengthPredator {
public:
  /**
   * \brief This is the NumberPredator constructor
   * \param infile is the CommentStream to read the predation parameters from
   * \param givenname is the name of the predator
   * \param Areas is the IntVector of areas that the predator lives on
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param multscaler is the Formula that can be used to scale the biomass consumed
   */
  NumberPredator(CommentStream& infile, const char* givenname, const IntVector& Areas,
    const TimeClass* const TimeInfo, Keeper* const keeper, Formula multscaler);
  /**
   * \brief This is the default NumberPredator destructor
   */
  virtual ~NumberPredator() {};
  /**
   * \brief This will calculate the amount the predator consumes on a given area
   * \param area is the area that the prey consumption is being calculated on
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Eat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This will adjust the amount the predator consumes on a given area, to take oversconsumption into consideration
   * \param area is the area that the prey consumption is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void adjustConsumption(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will print the predator information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This function will return the amount of prey on the area before consumption
   * \param area is the area that the prey consumption is being calculated on
   * \param preyname is the name of the prey that is being consumed
   * \return the population of the prey for the area before consumption
   */
  virtual const PopInfoVector& getNumberPriorToEating(int area, const char* preyname) const;
};

#endif