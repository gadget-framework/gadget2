#ifndef fleet_h
#define fleet_h

#include "base.h"
#include "formulamatrix.h"

class LengthPredator;
class PopPredator;
class AreaClass;
class TimeInfo;
class Keeper;

typedef int FleetType;
const FleetType TOTALAMOUNTGIVEN = 1;
const FleetType LINEARFLEET      = 2;
const FleetType MORTALITYFLEET   = 3;

class Fleet : public BaseClass {
public:
  Fleet(CommentStream& infile, const char* givenname,
    const AreaClass* const Area, const TimeClass* const TimeInfo,
    Keeper* const keeper, FleetType type);
  virtual ~Fleet();
  void Print(ofstream& outfile) const;
  virtual void CalcNumbers(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void ReducePop(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void Grow(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void FirstUpdate(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void SecondUpdate(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void ThirdUpdate(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void FirstSpecialTransactions(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void SecondSpecialTransactions(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void Migrate(const TimeClass* const TimeInfo);
  virtual void RecalcMigration(const TimeClass* const TimeInfo);
  virtual void CalcEat(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void CheckEat(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void AdjustEat(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void Reset(const TimeClass* const TimeInfo);
  LengthPredator* ReturnPredator() const;
  double OverConsumption(int area) const;
  double Amount(int area, const TimeClass* const TimeInfo) const {
    return amount[TimeInfo->CurrentTime()][AreaNr[area]];
  };
protected:
  LengthPredator* predator;
  FormulaMatrix amount;
  FleetType fleetType;
};

#endif
