#ifndef lenstock_h
#define lenstock_h

#include "commentstream.h"
#include "lennaturalm.h"
#include "cannibalism.h"
#include "mortpredlength.h"
#include "stock.h"
#include "stockptrvector.h"

class LenStock;
class LenNaturalM;
class Cannibalism;
class MortPredLength;
class MortPrey;
class StockRecruitment;
class Maturity;
class RenewalData;
class Spawner;
class LengthGroupDivision;
class ConversionIndex;

class LenStock : public Stock {
public:
  LenStock(CommentStream& infile, const char* givenname, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  virtual ~LenStock();
  stock_type stockType() const { return LENSTOCK_TYPE; };
  virtual void CalcNumbers(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void ReducePop(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void CalcEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void CheckEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void AdjustEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void Grow(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void Reset(const TimeClass* const TimeInfo);
  virtual void SecondSpecialTransactions(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void SetStock(Stockptrvector& stockvec);
  Prey* ReturnPrey() const;
  Predator* ReturnPredator() const;
  void Print(ofstream& outfile) const;
  int HasCannibalism () const { return cannibalism; };
  const doublevector& mortality() const { return len_natm->NatMortality(); };
  void calcForPrinting(int, const TimeClass&);
  const doublematrix& getF(int area) const { return *(F[area]); };
  const doublematrix& getM1(int area) const { return *(M1[area]); };
  const doublematrix& getM2(int area) const { return *(M2[area]); };
  const doublematrix& getNbar(int area) const { return *(Nbar[area]); };
  const doublematrix& getNsum(int area) const { return *(Nsum[area]); };
  const doublevector* getN(int area) const { return new doublevector(N); };
  const doublematrix& getBiomass(int area) const { return *(bio[area]); };
  void calcBiomass(int yr, int area);
protected:
  LenNaturalM* len_natm;
  Cannibalism* cann;
  int cannibalism;
private:
  Stockptrvector cannPredators;
  doublevector cann_vec;
  doublematrixptrvector F;
  doublematrixptrvector M1;
  doublematrixptrvector M2;
  doublematrixptrvector Nbar;
  doublematrixptrvector Nsum;
  doublematrixptrvector bio;
  doublevector C;
  doublevector D1;
  doublevector D2;
  doublevector N;
  doublevector E;
  doublevector Z;
  int year;
  double filter;
  int calcDone;
};

#endif
