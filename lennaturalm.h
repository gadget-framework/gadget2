#ifndef lennaturalm_h
#define lennaturalm_h

#include "areatime.h"
#include "commentstream.h"
#include "conversion.h"
#include "formulavector.h"

class LenNaturalM;
class Keeper;

class LenNaturalM {
public:
  LenNaturalM(CommentStream& infile, const LengthGroupDivision* lenp, Keeper* const keeper);
  ~LenNaturalM();
  void Print(ofstream& outfile);
  inline const DoubleVector& NatMortality() { return natmort; };
  void NatCalc();
protected:
  double Hyperbola(double start, double end, double startm, double endm, double len);
  FormulaVector parammort;
  DoubleVector xparammort;
  DoubleVector natmort;
  LengthGroupDivision* lengroup;
};

#endif
