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
  virtual void SetStock(StockPtrVector& stockvec);
  Prey* ReturnPrey() const;
  Predator* ReturnPredator() const;
  void Print(ofstream& outfile) const;
  int HasCannibalism() const { return cannibalism; };
  const DoubleVector& mortality() const { return len_natm->NatMortality(); };
  void calcForPrinting(int, const TimeClass&);
  const DoubleMatrix& getF(int area) const { return *(F[area]); };
  const DoubleMatrix& getM1(int area) const { return *(M1[area]); };
  const DoubleMatrix& getM2(int area) const { return *(M2[area]); };
  const DoubleMatrix& getNbar(int area) const { return *(Nbar[area]); };
  const DoubleMatrix& getNsum(int area) const { return *(Nsum[area]); };
  const DoubleVector* getN(int area) const { return new DoubleVector(N); };
  const DoubleMatrix& getBiomass(int area) const { return *(bio[area]); };
  void calcBiomass(int yr, int area);
protected:
  LenNaturalM* len_natm;
  Cannibalism* cann;
  int cannibalism;
private:
  StockPtrVector cannPredators;
  DoubleVector cann_vec;
  DoubleMatrixPtrVector F;
  DoubleMatrixPtrVector M1;
  DoubleMatrixPtrVector M2;
  DoubleMatrixPtrVector Nbar;
  DoubleMatrixPtrVector Nsum;
  DoubleMatrixPtrVector bio;
  DoubleVector C;
  DoubleVector D1;
  DoubleVector D2;
  DoubleVector N;
  DoubleVector E;
  DoubleVector Z;
  int year;
  double filter;
  int calcDone;
};

#endif
