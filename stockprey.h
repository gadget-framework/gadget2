#ifndef stockprey_h
#define stockprey_h

#include "prey.h"
#include "agebandmatrixptrvector.h"

class StockPrey;

class StockPrey : public Prey {
public:
  StockPrey(CommentStream& infile, const IntVector& areas, const char* givenname,
    int minage, int maxage, Keeper* const keeper);
  StockPrey(const DoubleVector& lengths, const IntVector& areas,
    int minage, int maxage, const char* givenname);
  virtual ~StockPrey();
  virtual PreyType preyType() const { return STOCKPREYTYPE; };
  virtual void Sum(const AgeBandMatrix& Alkeys, int area, int NrofSubstep);
  const AgeBandMatrix& AlkeysPriorToEating(int area) const;
  virtual void Print(ofstream& outfile) const;
  virtual void Reset();
protected:
  AgeBandMatrixPtrVector Alkeys;
};

#endif
