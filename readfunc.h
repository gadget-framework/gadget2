#ifndef readfunc_h
#define readfunc_h

#include "doubleindexvector.h"
#include "doublematrix.h"
#include "intmatrix.h"
#include "stockptrvector.h"
#include "charptrvector.h"
#include "commentstream.h"

class AreaClass;
class TimeClass;
class Stock;
class OtherFood;
class Keeper;

DoubleMatrix* ReadMatrix(CommentStream& infile, int x, int y);
int ReadMatrix(CommentStream& infile, DoubleMatrix& M);
DoubleVector* ReadVector(CommentStream& infile, int length);
int ReadVector(CommentStream& infile, IntVector& Vec);
int ReadVector(CommentStream& infile, DoubleVector& Vec);
int Read2ColVector(CommentStream& infile, DoubleMatrix& M);
DoubleIndexVector* ReadIndexVector(CommentStream& infile, int length, int min);
int ReadIndexVector(CommentStream& infile, DoubleIndexVector& Vec);
int ReadTextInLine(CommentStream& infile, CharPtrVector& text);
int ReadVectorInLine(CommentStream& infile, IntVector& Vec);
int ReadVectorInLine(CommentStream& infile, DoubleVector& Vec);
int FindContinuousYearAndStepWithNoText(CommentStream& infile, int year, int step);
int CountColumns(CommentStream& infile);

#endif



