#ifndef totalpredator_h
#define totalpredator_h

#include "lengthpredator.h"

/**
 * \class TotalPredator
 * \brief This is the class used to model the predation of stocks by a fleet that will catch a given biomass
 */
class TotalPredator : public LengthPredator {
public:
  /**
   * \brief This is the TotalPredator constructor
   * \param infile is the CommentStream to read the predation parameters from
   * \param givenname is the name of the predator
   * \param Areas is the IntVector of areas that the predator lives on
   * \param OtherLgrpDiv is the LengthGroupDivision that the predation will be applied to
   * \param GivenLgrpDiv is the LengthGroupDivision that will be used to calculate the predation
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param multi is the Formula that can be used to scale the biomass consumed
   * \note There are 2 LengthGroupDivision objects specified - GivenLgrpDiv will be used to calculate the predation, which will then be applied to OtherLgrpDiv.  GivenLgrpDiv must not be finer than OtherLgrpDiv
   */
  TotalPredator(CommentStream& infile, const char* givenname, const IntVector& Areas,
    const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv,
    const TimeClass* const TimeInfo, Keeper* const keeper, Formula multi);
  /**
   * \brief This is the default TotalPredator destructor
   */
  virtual ~TotalPredator() {};
  /**
   * \brief This will calculate the amount the predator consumes for a given area and timestep
   * \param area is the area that the prey consumption is being calculated on
   * \param LengthOfStep is the length of the timestep (not used in this calculation)
   * \param Temperature is the temperature of the area (not used in this calculation)
   * \param Areasize is the size of the area (not used in this calculation)
   * \param CurrentSubstep is the substep of the current timestep (so this calculation takes place once per timestep)
   * \param numsubsteps is the number of substeps in the current timestep
   */
  virtual void Eat(int area, double LengthOfStep, double Temperature,
    double Areasize, int CurrentSubstep, int numsubsteps);
  /**
   * \brief This will adjust the amount the predator consumes for a given area and timestep, to take oversconsumption into consideration
   * \param area is the area that the prey consumption is being calculated on
   * \param CurrentSubstep is the substep of the current timestep (so this calculation takes place once per timestep)
   * \param numsubsteps is the number of substeps in the current timestep
   */
  virtual void adjustConsumption(int area, int numsubsteps, int CurrentSubstep);
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
  const PopInfoVector& getNumberPriorToEating(int area, const char* preyname) const;
};

#endif
