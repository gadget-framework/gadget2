#ifndef fleet_h
#define fleet_h

#include "base.h"
#include "formulamatrix.h"
#include "lengthpredator.h"
#include "keeper.h"

enum FleetType { TOTALFLEET = 1, LINEARFLEET, NUMBERFLEET, EFFORTFLEET, QUOTAFLEET };

/**
 * \class Fleet
 * \brief This is the class used to model the predation that is due to the action of a fleet in the Gadget model
 *
 * This class calculates the catch of the fleets that are to be modelled within the simulation.  The fleets are treated as (non-dynamic) predators that ''eat'' the stock that is defined as the prey of the fleet.  The amount of the stock that is eaten is calculated by the predator that is associated with the fleet.  There are 5 types of fleet, each with a different predator class to calculate the level of predation.  A fleet that has been declared as a TOTALFLEET uses the TotalPredator class to calculate the predation, a LINEARFLEET uses the LinearPredator class to calculate the predation, a NUMBERFLEET uses the NumberPredator class to calculate the predation by number, a EFFORTFLEET uses the EffortPredator class to calculate the predation based on effort data and a QUOTAFLEET uses the QuotaPredator class to calculate the fishing quota based on the biomass of stock available, and then uses this calculated fishing level to calculate the predation of the stock.
 */
class Fleet : public BaseClass {
public:
  /**
   * \brief This is the Fleet constructor
   * \param infile is the CommentStream to read the fleet parameters from
   * \param givenname is the name of the fleet
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   * \param ftype is the FleetType of the fleet
   */
  Fleet(CommentStream& infile, const char* givenname, const AreaClass* const Area,
     const TimeClass* const TimeInfo, Keeper* const keeper, FleetType ftype);
  /**
   * \brief This is the default Fleet destructor
   */
  virtual ~Fleet();
  /**
   * \brief This function will print the model population
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile) const;
  /**
   * \brief This function will calculate the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void calcEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will check the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void checkEat(int area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will adjust the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void adjustEat(int area, const TimeClass* const TimeInfo);
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
   * \brief This will return the fleet, as a predator type, for use in other functions
   * \return predator
   */
  LengthPredator* getPredator() const { return predator; };
  /**
   * \brief This will check if the fleet will operate on the current timestep
   * \param area is the area that the fleet will operate on
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the fleet will operate, 0 otherwise
   */
  int isFleetStepArea(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This will return the type of fleet class
   * \return type
   */
  FleetType getType() const { return type; };
protected:
  /**
   * \brief This is the fleet, as a predator in the current model
   */
  LengthPredator* predator;
  /**
   * \brief This is the amount of the preys that the fleet has consumed
   */
  FormulaMatrix amount;
  /**
   * \brief This is the PopInfoMatrix used to temporarily store the amount consumed
   */
  PopInfoMatrix tmpPopulation;
private:
  /**
   * \brief This denotes what type of fleet class has been created
   */
  FleetType type;
};

#endif
