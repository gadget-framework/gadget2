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
  void setPrey(PreyPtrVector& preyvec, Keeper* const keeper);
  int doesEat(const char* preyname) const;
  /**
   * \brief This will adjust the amount the predator consumes on a given area to take any overconsumption into account
   * \param area is the area that the prey consumption is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   * \note This will be overridden by the derived classes that actually calculate the consumption
   */
  virtual void adjustConsumption(int area, const TimeClass* const TimeInfo) = 0;
  virtual void Print(ofstream& outfile) const;
  virtual const BandMatrix& getConsumption(int area, const char* preyname) const = 0;
  virtual const DoubleVector& getOverConsumption(int area) const = 0;
  virtual double getTotalOverConsumption(int area) const = 0;
  virtual const PopInfoVector& getNumberPriorToEating(int area, const char* preyname) const = 0;
  virtual const LengthGroupDivision* getLengthGroupDiv() const = 0;
  virtual int numLengthGroups() const = 0;
  virtual double meanLength(int i) const = 0;
  virtual void Reset(const TimeClass* const TimeInfo);
  const char* getPreyName(int i) const { return Suitable->getPreyName(i); };
  const BandMatrix& Suitability(int i) const { return Suitable->getSuitability(i); };
  int numPreys() const { return Suitable->numPreys(); };
  Prey* getPrey(int i) const { return preys[i]; };
  int didChange(int i, const TimeClass* const TimeInfo) const { return Suitable->didChange(i, TimeInfo); };
protected:
  void readSuitability(CommentStream& infile, const char* strFinal,
    const TimeClass* const TimeInfo, Keeper* const keeper);
private:
  /**
   * \brief This is the PreyPtrVector of the preys that will be consumed by the predator
   */
  PreyPtrVector preys;
  Suits* Suitable;
};

#endif
