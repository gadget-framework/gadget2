#ifndef transition_h
#define transition_h

#include "commentstream.h"
#include "stock.h"

class TimeClass;
class LengthGroupDivision;
class ConversionIndex;
class Keeper;

class Transition : protected LivesOnAreas {
public:
  Transition(CommentStream& infile, const intvector& areas, int age, int minl,
    int size, Keeper* const keeper);
  ~Transition();
  void SetCI(const LengthGroupDivision* const GivenLDiv);
  void SetStock(Stockptrvector& stockvec);
  void KeepAgegroup(int area, Agebandmatrix& Alkeys, const TimeClass* const TimeInfo);
  void MoveAgegroupToTransitionStock(int area, const TimeClass* const TimeInfo, int HasLgr);
  void Print(ofstream& outfile) const;
protected:
  int TransitionStep;
  char* TransitionStockName;
  Stock* TransitionStock;
  Agebandmatrixvector Agegroup;
  ConversionIndex* CI;
  int age;
  double minlen;
  int lenindex;
};

#endif
