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
  Transition(CommentStream& infile, const intvector& areas, int age,
    const LengthGroupDivision* const lgrpdiv, Keeper* const keeper);
  ~Transition();
  void SetStock(Stockptrvector& stockvec);
  void KeepAgegroup(int area, Agebandmatrix& Alkeys, const TimeClass* const TimeInfo);
  void MoveAgegroupToTransitionStock(int area, const TimeClass* const TimeInfo, int HasLgr);
  void Print(ofstream& outfile) const;
protected:
  Stockptrvector TransitionStocks;
  charptrvector TransitionStockNames;
  doublevector Ratio;
  ConversionIndexptrvector CI;
  LengthGroupDivision* LgrpDiv;
  int TransitionStep;
  //JMB agebandmatrixptrvector Agegroup;
  int age;
  doublevector minlen;
  intvector lenindex;
};

#endif
