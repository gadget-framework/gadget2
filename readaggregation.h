#ifndef readaggregation_h
#define readaggregation_h

#include "commentstream.h"
#include "doublevector.h"
#include "charptrvector.h"
#include "intmatrix.h"
#include "doublematrix.h"
#include "charptrmatrix.h"
#include "formulamatrix.h"

extern int readAggregation(CommentStream& infile, IntMatrix& agg, CharPtrVector& aggindex);
extern int readAggregation(CommentStream& infile, IntVector& agg, CharPtrVector& aggindex);
extern int readLengthAggregation(CommentStream& infile, DoubleVector& lengths, CharPtrVector& lenindex);
extern int readPreyAggregation(CommentStream& infile, CharPtrMatrix& preynames,
  DoubleMatrix& preylengths, FormulaMatrix& digestioncoeff, CharPtrVector& preyindex, Keeper* const keeper);

#endif
