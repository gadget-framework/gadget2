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

/**
 * \class Predator
 * \brief This is the base class used to model the consumption by a predator
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
  virtual void Reset(const TimeClass* const TimeInfo);
  /**
   * \brief This will return the amount the predator consumes of a given prey on a given area
   * \param area is the area that the consumption is being calculated on
   * \param preyname is the name of the prey that is being consumed
   * \return 0 (will be overridden by the derived classes)
   */
  virtual const BandMatrix& getConsumption(int area, const char* preyname) const = 0;
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
   * \brief This will return the amount of a given prey on a given area prior to any consumption by the predator
   * \param area is the area that the consumption is being calculated on
   * \param preyname is the name of the prey that is being consumed
   * \return 0 (will be overridden by the derived classes)
   */
  virtual const PopInfoVector& getNumberPriorToEating(int area, const char* preyname) const = 0;
  /**
   * \brief This will return the length group information for the predator
   * \return 0 (will be overridden by the derived classes)
   */
  virtual const LengthGroupDivision* getLengthGroupDiv() const = 0;
  /**
   * \brief This will return the number of prey stocks that the predator will consume
   * \return number of preys
   */
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
   * \brief This will return the suitability values for a given prey
   * \param i is the index of the prey
   * \return suitability matrix for the prey
   */
  const BandMatrix& Suitability(int i) const { return suitable->getSuitability(i); };
  /**
   * \brief This function will check to see if the suitability values for a given prey have changed
   * \param i is the index of the prey
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the values have changed, 0 otherwise
   */
  int didChange(int i, const TimeClass* const TimeInfo) const { return suitable->didChange(i, TimeInfo); };
protected:
  /**
   * \brief This function will read the suitability data from the input file
   * \param infile is the CommentStream to read the suitabiltiy data from
   * \param strFinal is the string that indicates the end of the suitability data
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  void readSuitability(CommentStream& infile, const char* strFinal,
    const TimeClass* const TimeInfo, Keeper* const keeper);
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
