#ifndef transition_h
#define transition_h

#include "areatime.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "stock.h"

class Keeper;

class Transition : protected LivesOnAreas {
public:
  Transition(CommentStream& infile, const IntVector& areas, int age,
    const LengthGroupDivision* const lgrpdiv, Keeper* const keeper);
  ~Transition();
  void SetStock(StockPtrVector& stockvec);
  void KeepAgegroup(int area, AgeBandMatrix& Alkeys, const TimeClass* const TimeInfo);
  void MoveAgegroupToTransitionStock(int area, const TimeClass* const TimeInfo, int HasLgr);
  void Print(ofstream& outfile) const;
protected:
  StockPtrVector TransitionStocks;
  CharPtrVector TransitionStockNames;
  DoubleVector Ratio;
  ConversionIndexPtrVector CI;
  LengthGroupDivision* LgrpDiv;
  int TransitionStep;
  AgeBandMatrixPtrVector Agegroup;
  int age;
};

#endif
