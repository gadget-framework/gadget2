#ifndef readfunc_h
#define readfunc_h

#include "doubleindexvector.h"
#include "doublematrix.h"
#include "intmatrix.h"
#include "otherfoodptrvector.h"
#include "stockptrvector.h"
#include "charptrvector.h"
#include "commentstream.h"

class AreaClass;
class TimeClass;
class Stock;
class OtherFood;
class Keeper;

doublematrix* ReadMatrix(CommentStream& infile, int x, int y);
int ReadMatrix(CommentStream& infile, doublematrix& M);
doublevector* ReadVector(CommentStream& infile, int length);
int ReadVector(CommentStream& infile, intvector& Vec);
int ReadVector(CommentStream& infile, doublevector& Vec);
int Read2ColVector(CommentStream& infile, doublematrix& M);
doubleindexvector* ReadIndexVector(CommentStream& infile, int length, int min);
int ReadIndexVector(CommentStream& infile, doubleindexvector& Vec);
int ReadTextInLine(CommentStream& infile, charptrvector& text);
int ReadVectorInLine(CommentStream& infile, intvector& Vec);
int ReadVectorInLine(CommentStream& infile, doublevector& Vec);
int FindContinuousYearAndStepWithNoText(CommentStream& infile, int year, int step);
int CountColumns(CommentStream& infile);

#endif



