#ifndef actionattimes_h
#define actionattimes_h

#include "commentstream.h"
#include "intvector.h"

class ActionAtTimes;
class TimeClass;

class ActionAtTimes {
public:
  ActionAtTimes();
  int ReadFromFile(CommentStream& infile, const TimeClass* const TimeInfo);
  void AddActions(const intvector& years, const intvector& steps, const TimeClass* const TimeInfo);
  void AddActionsAtAllYears(const intvector& steps, const TimeClass* const TimeInfo);
  void AddActionsAtAllSteps(const intvector& years, const TimeClass* const TimeInfo);
  int AtCurrentTime(const TimeClass* const TimeInfo) const;
protected:
  //Here, 'true' means that the function AtCurrentTime returns 1.
  int EveryStep;            //Is it true on every step?
  intvector TimeSteps;      //The timesteps on which it is true.
  intvector Years;          //The years on which it is always true.
  intvector Steps;          //The steps on which it is always true.
};

#endif
