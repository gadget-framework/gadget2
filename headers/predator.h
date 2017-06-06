#ifndef predator_h
#define predator_h

#include "areatime.h"
#include "lengthgroup.h"
#include "commentstream.h"
#include "preyptrvector.h"
#include "hasname.h"
#include "livesonareas.h"
#include "suits.h"
#include "keeper.h"
#include "prey.h"

enum PredatorType { STOCKPREDATOR = 1, TOTALPREDATOR, LINEARPREDATOR, NUMBERPREDATOR,
  EFFORTPREDATOR, QUOTAPREDATOR };

/**
 * \class Predator
 * \brief This is the base class used to model the consumption by a predator
 *
 * This class is used to calculate the predation of prey species by a predator, either by a fleet or by a modelled stock in the simulation.  The amount of the various prey species that the predator wants to consume is calculated, based on the suitability functions defined for each prey.   This ''target consumption'' is then checked to ensure that no more than 95% of each the prey species is to be consumed (by all the predators of that prey), and the consumption is adjusted if this is the case.  The population of the prey species is then reduced by the total amount that the predators consume.
 *
 * \note This will be overridden by the derived classes that actually calculate the consumption
 */
class Predator : public HasName, public LivesOnAreas {
public:
  /**
   * \brief This is the Predator constructor
   * \param givenname is the name of the predator
   * \param Areas is the IntVector of areas that the predator lives on
   */
  Predator(const char* givenname, const IntVector& Areas);
  /**
   * \brief This is the default Predator destructor
   */
  virtual ~Predator();
  /**
   * \brief This will calculate the amount the predator consumes on a given area
   * \param area is the area that the prey consumption is being calculated on
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \note This will be overridden by the derived classes that actually calculate the consumption
   */
  virtual void Eat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This will select the preys that will be consumed by the predator
   * \param preyvec is the PreyPtrVector of all the available preys
   * \param keeper is the Keeper for the current model
   */
  void setPrey(PreyPtrVector& preyvec, Keeper* const keeper);
  /**
   * \brief This function will check to see if the predator will consume a specified prey
   * \param preyname is the name of the prey
   * \return 1 if the predator does consume the prey, 0 otherwise
   */
  int doesEat(const char* preyname) const;
  /**
   * \brief This will adjust the amount the predator consumes on a given area to take any overconsumption into account
   * \param area is the area that the prey consumption is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   * \note This will be overridden by the derived classes that actually calculate the consumption
   */
  virtual void adjustConsumption(int area, const TimeClass* const TimeInfo) = 0;
  /**
   * \brief This function will print the predation data
   * \param outfile is the ofstream that all the model information gets sent to
   */
  virtual void Print(ofstream& outfile) const;
  /**
   * \brief This will reset the predation information for the current model run
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Reset(const TimeClass* const TimeInfo) { suitable->Reset(this, TimeInfo); };
  /**
   * \brief This will return the amount the predator consumes of a given prey on a given area
   * \param area is the area that the consumption is being calculated on
   * \param preyname is the name of the prey that is being consumed
   * \return 0 (will be overridden by the derived classes)
   */
  virtual const DoubleMatrix& getConsumption(int area, const char* preyname) const = 0;
  /**
   * \brief This will return the flag that denotes if the predator has overconsumed on a given area
   * \param area is the area that the consumption is being calculated on
   * \return 0 (will be overridden by the derived classes)
   */
  virtual int hasOverConsumption(int area) const = 0;
  /**
   * \brief This will return the amount the predator overconsumes on a given area
   * \param area is the area that the consumption is being calculated on
   * \return 0 (will be overridden by the derived classes)
   */
  virtual const DoubleVector& getOverConsumption(int area) const = 0;
  /**
   * \brief This will return the total amount the predator overconsumes on a given area
   * \param area is the area that the consumption is being calculated on
   * \return 0 (will be overridden by the derived classes)
   */
  virtual double getTotalOverConsumption(int area) const = 0;
  /**
   * \brief This will return the biomass the predator consumes of a given prey on a given area
   * \param prey is the index for the prey
   * \param area is the area that the consumption is being calculated on
   * \return 0 (will be overridden by the derived classes)
   */
  virtual double getConsumptionBiomass(int prey, int area) const = 0;
  /**
   * \brief This will return the ratio used to split the predation between the areas and length groups
   * \param area is the area that the consumption is being calculated on
   * \param prey is the index for the prey
   * \param len is the length group of the predator
   * \return 0 (will be overridden by the derived classes)
   */
  virtual double getConsumptionRatio(int area, int prey, int len) const = 0;
  /**
   * \brief This will return the amount of a given prey on a given area prior to any consumption by the predator
   * \param area is the area that the consumption is being calculated on
   * \param preyname is the name of the prey that is being consumed
   * \return 0 (will be overridden by the derived classes)
   */
  virtual const PopInfoVector& getConsumptionPopInfo(int area, const char* preyname) const = 0;
  /**
   * \brief This will return the length group information for the predator
   * \return 0 (will be overridden by the derived classes)
   */
  virtual const LengthGroupDivision* getLengthGroupDiv() const = 0;
  /**
   * \brief This is a multiplier set in lengthpredator.  
   */
  virtual void setTimeMultiplier(const TimeClass* const TimeInfo, int quotastep,double value){};
  /**
   * \brief This will return the number of prey stocks that the predator will consume
   * \return number of preys
   */
  /**
   * \brief This will initialise the preys that will be consumed by the predator
   */
  virtual void Initialise() { suitable->Initialise(this); };

  int numPreys() const { return preys.Size(); };
  /**
   * \brief This will return a given prey
   * \param i is the index of the prey
   * \return prey
   */
  Prey* getPrey(int i) const { return preys[i]; };
  /**
   * \brief This will return the name of a given prey
   * \param i is the index of the prey
   * \return name of the prey
   */
  const char* getPreyName(int i) const { return suitable->getPreyName(i); };
  /**
   * \brief This function will check to see if the suitability parameters for a given prey have changed on the current timestep
   * \param i is the index of the prey
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the values have changed, 0 otherwise
   */
  int didChange(int i, const TimeClass* const TimeInfo) { return suitable->didChange(i, TimeInfo); };
  /**
   * \brief This will return the suitability values for a given prey
   * \param i is the index of the prey
   * \return suitability matrix for the prey
   */
  const DoubleMatrix& getSuitability(int i) const { return suitable->getSuitability(i); };
  /**
   * \brief This will return the preference for a given prey
   * \param i is the index of the prey
   * \return preference
   */
  double getPreference(int i) const { return preference[i]; };
  /**
   * \brief This will return the type of predator class
   * \return type
   */
  PredatorType getType() const { return type; };
protected:
  /**
   * \brief This function will read the suitability data from the input file
   * \param infile is the CommentStream to read the suitabiltiy data from
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  void readSuitability(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the FormulaVector used to store the prey preference parameters
   */
  FormulaVector preference;
  /**
   * \brief This denotes what type of predator class has been created
   */
  PredatorType type;
private:
  /**
   * \brief This is the PreyPtrVector of the preys that will be consumed by the predator
   */
  PreyPtrVector preys;
  /**
   * \brief This is the Suits of the suitability values for the preys that will be consumed by the predator
   */
  Suits* suitable;
};

#endif
