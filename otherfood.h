#ifndef otherfood_h
#define otherfood_h

#include "formulamatrix.h"
#include "base.h"

class OtherFood;
class LengthPrey;

class OtherFood : public BaseClass {
public:
  OtherFood(CommentStream& infile, const char* givename,
    const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper);
  virtual ~OtherFood();
  virtual void CalcEat(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void CheckEat(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void AdjustEat(int area,
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
  virtual void CalcNumbers(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void Migrate(const TimeClass* const TimeInfo);
  virtual void RecalcMigration(const TimeClass* const TimeInfo);
  virtual void Reset(const TimeClass* const TimeInfo);
  LengthPrey* ReturnPrey() const;
  void Print(ofstream& outfile) const;
protected:
  FormulaMatrix amount;    //[time][area]
  LengthPrey* prey;
};

#endif
