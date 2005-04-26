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
  virtual void Eat(int area, double LengthOfStep, double Temperature, double Areasize,
    int CurrentSubstep, int numsubsteps) = 0;
  void setPrey(PreyPtrVector& preyvec, Keeper* const keeper);
  int doesEat(const char* preyname) const;
  virtual void adjustConsumption(int area, int numsubsteps, int CurrentSubstep) = 0;
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
  int DidChange(int i, const TimeClass* const TimeInfo) const { return Suitable->DidChange(i, TimeInfo); };
protected:
  void readSuitability(CommentStream& infile, const char* strFinal,
    const TimeClass* const TimeInfo, Keeper* const keeper);
private:
  PreyPtrVector preys;
  Suits* Suitable;
};

#endif
