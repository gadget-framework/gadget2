#ifndef cannibalism_h
#define cannibalism_h

#include "doubleindexvector.h"
#include "commentstream.h"
#include "bandmatrixptrvector.h"
#include "agebandmatrix.h"
#include "formulavector.h"
#include "intmatrix.h"

class Cannibalism;
class Keeper;
class TimeClass;

class Cannibalism {
public:
  Cannibalism(CommentStream& infile, const LengthGroupDivision* lenp,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  ~Cannibalism();
  void Print(ofstream& outfile);
  const DoubleVector& Mortality(const AgeBandMatrix& alk_prey,
    const AgeBandMatrix& alk_pred, const LengthGroupDivision* len_prey,
    const LengthGroupDivision* len_pred, const TimeClass* const TimeInfo,
    int pred_no, const DoubleVector& natm);
  int nrOfPredators() { return nrofpredators; };
  const char* predatorName(int pred) { return predatornames[pred]; };
  int getMinPredAge();
  int getMaxPredAge();
  int getMinPredAge(int i) { return minage[i]; };
  int getMaxPredAge(int i) { return maxage[i]; };
  const DoubleVector& getAgeGroups(int pred_no) { return agegroups[pred_no]; };
  const BandMatrix& getCons(int pred_no) { return *consumption[pred_no]; };
  double suitfunc(double predlen, double preylen);
protected:
  DoubleIndexVector altfood;
  IntMatrix overlap;    // overlap[predator][substep]
  CharPtrVector predatornames;
  int nrofpredators;
  Formula delta;
  FormulaVector params;
  FormulaVector cann_lev;
  DoubleVector cannibalism;
  IntVector minage;
  IntVector maxage;
  LengthGroupDivision* preylgp;
  DoubleMatrix agegroups; //[pred_no][ages] kgf 29/3/99
  BandMatrixPtrVector consumption;
};

#endif
