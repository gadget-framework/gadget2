#ifndef base_h
#define base_h

#include "areatime.h"
#include "hasname.h"
#include "livesonareas.h"

class BaseClass : public HasName, public LivesOnAreas {
public:
  BaseClass() {};
  BaseClass(const char* givenname) : HasName(givenname) {};
  BaseClass(const char* givenname, const IntVector& Areas) : HasName(givenname), LivesOnAreas(Areas) {};
  virtual ~BaseClass() {};
  virtual void CalcEat(int area, const AreaClass* const Area,
    const TimeClass* const TimeInfo) = 0;
  virtual void CheckEat(int area, const AreaClass* const Area,
    const TimeClass* const TimeInfo) = 0;
  virtual void AdjustEat(int area, const AreaClass* const Area,
    const TimeClass* const TimeInfo) = 0;
  virtual void ReducePop(int area, const AreaClass* const Area,
    const TimeClass* const TimeInfo) = 0;
  virtual void Grow(int area, const AreaClass* const Area,
    const TimeClass* const TimeInfo) = 0;
  virtual void FirstUpdate(int area, const AreaClass* const Area,
    const TimeClass* const TimeInfo) = 0;
  virtual void SecondUpdate(int area, const AreaClass* const Area,
    const TimeClass* const TimeInfo) = 0;
  virtual void ThirdUpdate(int area, const AreaClass* const Area,
    const TimeClass* const TimeInfo) = 0;
  virtual void FirstSpecialTransactions(int area, const AreaClass* const Area,
    const TimeClass* const TimeInfo) = 0;
  virtual void SecondSpecialTransactions(int area, const AreaClass* const Area,
    const TimeClass* const TimeInfo) = 0;
  virtual void CalcNumbers(int area, const AreaClass* const Area,
    const TimeClass* const TimeInfo) = 0;
  virtual void Migrate(const TimeClass* const TimeInfo) = 0;
  virtual void RecalcMigration(const TimeClass* const TimeInfo) = 0;
  virtual void Reset(const TimeClass* const TimeInfo) = 0;
  virtual void Clear() {};
  virtual void Print(ofstream& outfile) const = 0;
};

#endif
