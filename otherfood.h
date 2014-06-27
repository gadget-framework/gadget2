#ifndef otherfood_h
#define otherfood_h

#include "formulamatrix.h"
#include "lengthprey.h"
#include "base.h"

/**
 * \class OtherFood
 * \brief This is the class used to model the available prey that is not modelled dynamically
 *
 * This class calculates the amount of available non-dynamic prey for stocks to eat.  This is to account for the consumption of prey species that are not being modelled in the current simulation
 *
 * \note other food is defined as the \b density of other prey available
 */
class OtherFood : public BaseClass {
public:
  /**
   * \brief This is the OtherFood constructor
   * \param infile is the CommentStream to read the other food parameters from
   * \param givenname is the name of the other food
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  OtherFood(CommentStream& infile, const char* givenname,
    const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default OtherFood destructor
   */
  virtual ~OtherFood();
  /**
   * \brief This function will calculate the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void calcEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will check the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void checkEat(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will adjust the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void adjustEat(int area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will reduce the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void reducePop(int area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will calculate the growth of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Grow(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will calculate any transition of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updateAgePart1(int area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will calculate the age increase of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updateAgePart2(int area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will implement the transiton of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updateAgePart3(int area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will calculate the spawning of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart1(int area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will calculate add the newly matured stock into the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart2(int area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will calculate add the new recruits into the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart3(int area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will calculate calculate the straying of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart4(int area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will calculate add the strayed stock into the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart5(int area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will update the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void calcNumbers(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will implement the migration of the model population for the model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Migrate(const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will reset the model population
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Reset(const TimeClass* const TimeInfo);
  /**
   * \brief This will check if there is otherfood available on the current timestep
   * \param area is the area that the otherfood will be consumed on
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if there is otherfood available, 0 otherwise
   */
  int isOtherFoodStepArea(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This will return the otherfood, as a prey type, for use in other functions
   * \return prey
   */
  LengthPrey* getPrey() const { return prey; };
  /**
   * \brief This function will print the model population
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile) const;
protected:
  /**
   * \brief This is the amount of the otherfood that is available for the predators to consume
   */
  FormulaMatrix amount;
  /**
   * \brief This is the otherfood, as a prey in the current model
   */
  LengthPrey* prey;
  /**
   * \brief This is the PopInfoMatrix used to temporarily store the amount of food available
   */
  PopInfoMatrix tmpPopulation;
};

#endif
