#ifndef predator_h
#define predator_h

#include "commentstream.h"
#include "preyptrvector.h"
#include "hasname.h"
#include "livesonareas.h"
#include "popinfovector.h"
#include "suits.h"

class Predator;
class Keeper;
class LengthGroupDivision;
class Prey;
class AreaClass;

class Predator : public HasName, public LivesOnAreas {
public:
  Predator(const char* givenname, const intvector& Areas);
  virtual ~Predator();
  virtual void Eat(int area, double LengthOfStep, double Temperature, double Areasize,
    int CurrentSubstep, int NrOfSubsteps) = 0;
  void SetPrey(Preyptrvector& preyvec, Keeper* const keeper);
  int DoesEat(const char* preyname) const;
  virtual void AdjustConsumption(int area, int NrOfSubsteps, int CurrentSubstep) = 0;
  virtual void Print(ofstream& outfile) const;
  virtual const bandmatrix& Consumption(int area, const char* preyname) const = 0;
  virtual const doublevector& Consumption(int area) const = 0;
  virtual const doublevector& OverConsumption(int area) const = 0;
  virtual const popinfovector& NumberPriortoEating(int area, const char* preyname) const = 0;
  virtual const LengthGroupDivision* ReturnLengthGroupDiv() const = 0;
  virtual int NoLengthGroups() const = 0;
  virtual double Length(int i) const = 0;
  virtual void Reset(const TimeClass* const TimeInfo);
  const bandmatrix& Suitability(int i) const { return Suitable->Suitable(i); };
  int NoPreys() const { return Suitable->NoPreys(); };
  Prey* Preys(int i) const { return (Prey*)(preys[i]); };
  int DidChange(int prey, const TimeClass* const TimeInfo) const {
    return Suitable->DidChange(prey, TimeInfo);
  };
protected:
  virtual void DeleteParametersForPrey(int prey, Keeper* const keeper);
  virtual void ResizeObjects();
  void SetSuitability(const Suits* const S, Keeper* const keeper);
  const char* Preyname(int i) const { return Suitable->Preyname(i); };
  int ReadSuitabilityMatrix(CommentStream& infile, const char* FinalString,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  friend class Suits;
private:
  Preyptrvector preys;
  Suits* Suitable;
};

#endif
