#ifndef readmatrix_h
#define readmatrix_h

#include "commentstream.h"
#include "formulamatrixptrvector.h"

class TimeClass;
class Keeper;

int ReadAmounts(CommentStream& infile, const IntVector& tmpareas,
  const TimeClass* const TimeInfo, const AreaClass* const Area,
  FormulaMatrix& amount, Keeper* const keeper, const char* givenname);
int ReadGrowthAmounts(CommentStream& infile, const TimeClass* const TimeInfo,
  const AreaClass* const Area, FormulaMatrixPtrVector& amount,
  const CharPtrVector& lenindex, Keeper* const keeper);
int ReadContinuousTimeMatrix(CommentStream& infile, int NumberOfColumns,
  const TimeClass* const TimeInfo, FormulaMatrix& amount, Keeper* const keeper);

#endif
