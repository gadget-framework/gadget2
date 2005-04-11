#ifndef lengthpredator_h
#define lengthpredator_h

#include "poppredator.h"

/**
 * \class LengthPredator
 * \brief This is the class used to model the predation of stocks by a fleet
 */
class LengthPredator : public PopPredator {
public:
  /**
   * \brief This is the LengthPredator constructor
   * \param givenname is the name of the predator
   * \param Areas is the IntVector of areas that the predator lives on
   * \param OtherLgrpDiv is the LengthGroupDivision that the predation will be applied to
   * \param GivenLgrpDiv is the LengthGroupDivision that will be used to calculate the predation
   * \param keeper is the Keeper for the current model
   * \param multi is the Formula that can be used to scale the biomass consumed
   * \note There are 2 LengthGroupDivision objects specified - GivenLgrpDiv will be used to calculate the predation, which will then be applied to OtherLgrpDiv.  GivenLgrpDiv must not be finer than OtherLgrpDiv
   */
  LengthPredator(const char* givenname, const IntVector& Areas,
    const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv,
    Keeper* const keeper, Formula multi);
  /**
   * \brief This is the default LengthPredator destructor
   */
  virtual ~LengthPredator() {};
  /**
   * \brief This will calculate the amount of prey that is consumed by the predator
   * \param NumberInArea is the PopInfoVector giving the amount of prey in the area
   * \param area is the area that the prey consumption is being calculated on
   */
  virtual void Sum(const PopInfoVector& NumberInArea, int area);
  /**
   * \brief This will return a scaler used to split the catch between the areas
   * \param area is the area that the prey consumption is being calculated on
   * \return scaler
   */
  double Scaler(int area) const { return scaler[this->areaNum(area)]; };
  /**
   * \brief This will return the multiplicative value used scale the catch
   * \return Multiplicative
   */
  double multScaler() const { return Multiplicative; };
  /**
   * \brief This function will reset the predation information
   * \param TimeInfo is the KeeTimeClasser for the current model
   */
  virtual void Reset(const TimeClass* const TimeInfo);
protected:
  /**
   * \brief This is the DoubleVector of scalers used to split the total catch between the areas of the prey
   */
  DoubleVector scaler;
  /**
   * \brief This is the multiplicative constant that can be used to scale the biomass consumed
   */
  Formula Multiplicative;
};

#endif
