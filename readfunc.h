#ifndef readfunc_h
#define readfunc_h

#include "doublematrix.h"
#include "formulamatrixptrvector.h"
#include "commentstream.h"

DoubleMatrix* readMatrix(CommentStream& infile, int x, int y);
int readMatrix(CommentStream& infile, DoubleMatrix& M);
DoubleVector* readVector(CommentStream& infile, int length);
int readVector(CommentStream& infile, IntVector& Vec);
int readVector(CommentStream& infile, DoubleVector& Vec);
int read2ColVector(CommentStream& infile, DoubleMatrix& M);
DoubleIndexVector* readIndexVector(CommentStream& infile, int length, int min);
int readIndexVector(CommentStream& infile, DoubleIndexVector& Vec);
int readVectorInLine(CommentStream& infile, IntVector& Vec);
int readVectorInLine(CommentStream& infile, DoubleVector& Vec);
int FindContinuousYearAndStepWithNoText(CommentStream& infile, int year, int step);
int countColumns(CommentStream& infile);

int readAmounts(CommentStream& infile, const IntVector& tmpareas,
  const TimeClass* const TimeInfo, const AreaClass* const Area,
  FormulaMatrix& amount, Keeper* const keeper, const char* givenname);
int readGrowthAmounts(CommentStream& infile, const TimeClass* const TimeInfo,
  const AreaClass* const Area, FormulaMatrixPtrVector& amount,
  const CharPtrVector& lenindex, Keeper* const keeper);

#endif



