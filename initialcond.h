#ifndef initialcond_h
#define initialcond_h

#include "formulamatrix.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "agebandmatrix.h"
#include "agebandmatrixptrvector.h"
#include "keeper.h"

class Stock;

class InitialCond {
public:
  InitialCond(CommentStream& infile, const IntVector& area, Keeper* const keeper,
     const char* refWeightFile, const AreaClass* const Area);
  ~InitialCond();
  void Initialize(AgeBandMatrixPtrVector& Alkeys);
  void SetCI(const LengthGroupDivision* const GivenLDiv);
  void Print(ofstream& outfile) const;
protected:
  void ReadMeanData(CommentStream& infile, Keeper* const keeper,
     int noagegr, int minage, const AreaClass* const Area);
  void ReadNumberData(CommentStream& infile, Keeper* const keeper,
     int noagegr, int nolengr, int minage, const AreaClass* const Area);
  LengthGroupDivision* LgrpDiv;
  ConversionIndex* CI;
  AgeBandMatrixPtrVector AreaAgeLength;
  FormulaMatrix AreaDist;
  FormulaMatrix AgeDist;
  FormulaMatrix RelCondition;
  FormulaMatrix Mean;
  FormulaMatrix Sdev;
  Formula SdevMult;
  IntVector areas;
  int readNumbers;
};

#endif
