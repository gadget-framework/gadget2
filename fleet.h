#ifndef fleet_h
#define fleet_h

#include "base.h"
#include "formulamatrix.h"

class LengthPredator;
class PopPredator;
class Keeper;

enum FleetType { TOTALFLEET = 1, LINEARFLEET, MORTALITYFLEET };

/**
 * \class Fleet
 * \brief This is the class used to model the predation that is due to the action of a fleet in the Gadget model
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
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void checkEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will adjust the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void adjustEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will reduce the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void ReducePop(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {};
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
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void FirstUpdate(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will calculate the age increase of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void SecondUpdate(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will implement the transiton of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void ThirdUpdate(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the spawning of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void FirstSpecialTransactions(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will calculate the maturity and recruits of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void SecondSpecialTransactions(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will update the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void CalcNumbers(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will implement the migration of the model population for the model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Migrate(const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will recalculate the migration of the model population for the model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void RecalcMigration(const TimeClass* const TimeInfo) {};
  /**
   * \brief This function will reset the model population
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Reset(const TimeClass* const TimeInfo);
  /**
   * \brief This will return the fleet, as a predator type, for use in other functions
   * \return predator
   */
  LengthPredator* returnPredator() const;
  /**
   * \brief This will return the amount that the fleet has over consumed, for a given area
   * \param area is an integer to denote the internal area of interest
   * \return amount of overconsumption
   */
  double OverConsumption(int area) const;
  /**
   * \brief This will return the amount that the fleet has consumed, for a given area and time step
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   * \return amount
   */
  double Amount(int area, const TimeClass* const TimeInfo) const;
  /**
   * \brief This will return the type of fleet class
   * \return type
   */
  FleetType Type() { return type; };
protected:
  /**
   * \brief This is the fleet, as a predator in the current model
   */
  LengthPredator* predator;
  /**
   * \brief This is the amount of the preys that the fleet has consumed
   */
  FormulaMatrix amount;
private:
  /**
   * \brief This denotes what type of fleet class has been created
   */
  FleetType type;
};

#endif
