#ifndef readaggregation_h
#define readaggregation_h

#include "commentstream.h"
#include "doublevector.h"
#include "charptrvector.h"
#include "intmatrix.h"
#include "doublematrix.h"
#include "charptrmatrix.h"
#include "formulamatrix.h"

extern int ReadAggregation(CommentStream& infile, intmatrix& agg, charptrvector& aggindex);
extern int ReadAggregation(CommentStream& infile, intvector& agg, charptrvector& aggindex);
extern int ReadLengthAggregation(CommentStream& infile, doublevector& lengths, charptrvector& lenindex);
extern int ReadPreyAggregation(CommentStream& infile, charptrmatrix& preynames,
  doublematrix& preylengths, Formulamatrix& digestioncoeff, charptrvector& preyindex, Keeper* const keeper);

#endif
