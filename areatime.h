#ifndef areatime_h
#define areatime_h

#include "doublematrix.h"
#include "doubleindexvector.h"
#include "commentstream.h"

class AreaClass;
class TimeClass;

class AreaClass {
public:
  AreaClass(CommentStream& infile, const TimeClass* const TimeInfo);
  int NoAreas() const { return OuterAreas.Size(); };
  double Size(int area) const { return size[area]; };
  double Temperature(int area, int time) const { return temperature[time][area]; };
  int InnerArea(int area) const;
  int OuterArea(int area) const;
protected:
  intvector OuterAreas;
  doublevector size;
  doublematrix temperature;
};

class TimeClass {
public:
  TimeClass(CommentStream& infile);
  int CurrentSubstep() const { return currentsubstep; };
  int CurrentStep() const { return currentstep; };
  int CurrentYear() const { return currentyear; };
  int CurrentTime() const { return this->CalcSteps(currentyear, currentstep); };
  int FirstStep() const { return firststep; };
  int FirstYear() const { return firstyear; };
  int LastStep() const { return laststep; };
  int LastYear() const { return lastyear; };
  double LengthOfCurrent() const { return timesteps[currentstep]; };
  double LengthOfYear() const { return lengthofyear; };
  double LengthOfStep(int step) const { return timesteps[step]; };
  int CalcSteps(int year, int step) const {
    return (notimesteps * (year - firstyear) + step - firststep + 1); };
  int TotalNoSteps() const {
    return (notimesteps * (lastyear - firstyear) + laststep - firststep + 1); };
  int StepsInYear() const { return notimesteps; };
  void IncrementTime();
  int IsWithinPeriod(int year, int step) const;
  void ResetToBeginning() { currentyear = firstyear; currentstep = firststep; currentsubstep = 1; };
  int NrOfSubsteps() const;
  void IncrementSubstep() { currentsubstep++; };
  int SizeOfStepDidChange() const;
protected:
  int currentstep;
  int currentyear;
  int laststep;
  int firstyear;
  int lastyear;
  int firststep;
  int notimesteps;
  doubleindexvector timesteps;
  doubleindexvector nrofsubsteps;
  double lengthofyear;
  int currentsubstep;
};

#endif
