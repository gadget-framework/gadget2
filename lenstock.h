#ifndef lenstock_h
#define lenstock_h

#include "stock.h"
#include "mortprey.h"
#include "mortpredator.h"

class LenStock;
class LenNaturalM;
class Cannibalism;

class LenStock : public Stock {
public:
  LenStock(CommentStream& infile, const char* givenname, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  virtual ~LenStock();
  virtual void CalcNumbers(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void ReducePop(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void calcEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void checkEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void adjustEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void Grow(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void Reset(const TimeClass* const TimeInfo);
  virtual void SecondSpecialTransactions(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void setStock(StockPtrVector& stockvec);
  Prey* returnPrey() const;
  Predator* returnPredator() const;
  void Print(ofstream& outfile) const;
  int HasCannibalism() const { return cannibalism; };
  void calcForPrinting(int, const TimeClass&);
  const DoubleMatrix& getF(int area) const { return *(F[area]); };
  const DoubleMatrix& getM1(int area) const { return *(M1[area]); };
  const DoubleMatrix& getM2(int area) const { return *(M2[area]); };
  const DoubleMatrix& getNbar(int area) const { return *(Nbar[area]); };
  const DoubleMatrix& getNsum(int area) const { return *(Nsum[area]); };
  const DoubleMatrix& getBiomass(int area) const { return *(bio[area]); };
  void calcBiomass(int yr, int area);
protected:
  LenNaturalM* len_natm;
  Cannibalism* cann;
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
  int cannibalism;
};

#endif
