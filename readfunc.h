#ifndef readfunc_h
#define readfunc_h

#include "doublematrix.h"
#include "formulamatrixptrvector.h"
#include "commentstream.h"

int readMatrix(CommentStream& infile, DoubleMatrix& M);
int readVector(CommentStream& infile, DoubleVector& Vec);
int readIndexVector(CommentStream& infile, DoubleIndexVector& Vec);
int readVectorInLine(CommentStream& infile, IntVector& Vec);
int readVectorInLine(CommentStream& infile, DoubleVector& Vec);
int countColumns(CommentStream& infile);
int FindContinuousYearAndStepWithNoText(CommentStream& infile, int year, int step);

int readRefWeights(CommentStream& infile, DoubleMatrix& M);
int readAmounts(CommentStream& infile, const IntVector& tmpareas,
  const TimeClass* const TimeInfo, const AreaClass* const Area,
  FormulaMatrix& amount, Keeper* const keeper, const char* givenname);
int readGrowthAmounts(CommentStream& infile, const TimeClass* const TimeInfo,
  const AreaClass* const Area, FormulaMatrixPtrVector& amount,
  const CharPtrVector& lenindex, Keeper* const keeper);

#endif



