#ifndef stockprey_h
#define stockprey_h

#include "prey.h"

class StockPrey;

class StockPrey : public Prey {
public:
  StockPrey(CommentStream& infile, const intvector& areas, const char* givenname,
    int minage, int maxage, Keeper* const keeper);
  StockPrey(const doublevector& lengths, const intvector& areas,
    int minage, int maxage, const char* givenname);
  virtual ~StockPrey();
  virtual prey_type preyType() const { return STOCKPREY_TYPE; };
  virtual void Sum(const Agebandmatrix& Alkeys, int area, int NrofSubstep);
  const Agebandmatrix& AlkeysPriorToEating(int area) const;
  virtual void Print(ofstream& outfile) const;
  virtual void Reset();
protected:
  Agebandmatrixvector Alkeys;
};

#endif
