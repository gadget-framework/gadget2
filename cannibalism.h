#ifndef cannibalism_h
#define cannibalism_h

#include "doubleindexvector.h"
#include "commentstream.h"
#include "doublematrixptrvector.h"
#include "bandmatrixptrvector.h"
#include "agebandm.h"
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
  const doublevector& Mortality(const Agebandmatrix& alk_prey,
    const Agebandmatrix& alk_pred, const LengthGroupDivision* len_prey,
    const LengthGroupDivision* len_pred, const TimeClass* const TimeInfo,
    int pred_no, const doublevector& natm);
  int nrOfPredators() { return nrofpredators; };
  const char* predatorName(int pred) { return predatornames[pred]; };
  int getMinPredAge();
  int getMaxPredAge();
  int getMinPredAge(int i) { return minage[i]; };
  int getMaxPredAge(int i) { return maxage[i]; };
  const doublevector& getAgeGroups(int pred_no) { return agegroups[pred_no]; };
  const bandmatrix& getCons(int pred_no) { return(*consumption[pred_no]); };
  double suitfunc(double predlen, double preylen);
protected:
  intmatrix* overlap;    // overlap[predator][substep]
  charptrvector predatornames;
  int nrofpredators;
  Formula delta;
  doubleindexvector altfood;
  Formulavector params;
  Formulavector cann_lev;
  doublevector cannibalism;
  intvector minage;
  intvector maxage;
  LengthGroupDivision* preylgp;
  doublematrix agegroups; //[pred_no][ages] kgf 29/3/99
  bandmatrixptrvector consumption;
};

#endif




