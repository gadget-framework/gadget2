#ifndef variableinfo_h
#define variableinfo_h

#include "migration.h"
#include "gadget.h"

//The class VariableInfo keeps information on the additions to the migration matrices.
//The class expects row and column to be between 1 and numareas, both inclusive.
class VariableInfo {
  Keeper* keeper;
public:
  VariableInfo(Keeper* Keep, int noareas)
    : keeper(Keep), nummatrix(-1), row(-1), column(-1), error(0), numareas(noareas) {};
  ~VariableInfo() {};
  int nummatrix;
  int row;
  int column;
  IntVector indices;
  DoubleVector coefficients;
  int error;
  int numareas;
  friend CommentStream& operator >> (CommentStream& infile, VariableInfo& varinfo);
  friend ostream& operator << (ostream& out, VariableInfo& varinfo);
};

#endif
