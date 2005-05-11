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
  Predator(const char* givenname, const IntVector& Areas);
  virtual ~Predator();
  /**
   * \brief This will calculate the amount the predator consumes on a given area
   * \param area is the area that the prey consumption is being calculated on
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Eat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) = 0;
  void setPrey(PreyPtrVector& preyvec, Keeper* const keeper);
  int doesEat(const char* preyname) const;
  virtual void adjustConsumption(int area, const TimeClass* const TimeInfo) = 0;
  virtual void Print(ofstream& outfile) const;
  virtual const BandMatrix& getConsumption(int area, const char* preyname) const = 0;
  virtual const DoubleVector& getOverConsumption(int area) const = 0;
  virtual double getTotalOverConsumption(int area) const = 0;
  virtual const PopInfoVector& getNumberPriorToEating(int area, const char* preyname) const = 0;
  virtual const LengthGroupDivision* returnLengthGroupDiv() const = 0;
  virtual int numLengthGroups() const = 0;
  virtual double meanLength(int i) const = 0;
  virtual void Reset(const TimeClass* const TimeInfo);
  const char* getPreyName(int i) const { return Suitable->getPreyName(i); };
  const BandMatrix& Suitability(int i) const { return Suitable->Suitable(i); };
  int numPreys() const { return Suitable->numPreys(); };
  Prey* Preys(int i) const { return preys[i]; };
  int didChange(int i, const TimeClass* const TimeInfo) const { return Suitable->didChange(i, TimeInfo); };
protected:
  void readSuitability(CommentStream& infile, const char* strFinal,
    const TimeClass* const TimeInfo, Keeper* const keeper);
private:
  PreyPtrVector preys;
  Suits* Suitable;
};

#endif
