#ifndef variableinfo_h
#define variableinfo_h

#include "migration.h"
#include "gadget.h"

//The class VariableInfo keeps information on the additions to the migration matrices.
//The class expects row and column to be between 1 and NoAreas, both inclusive.
class VariableInfo {
  Keeper* keeper;
public:
  VariableInfo(Keeper* Keep, int noareas)
    : keeper(Keep), NoMatrix(-1), row(-1), column(-1), error(0), NoAreas(noareas) {};
  ~VariableInfo() {};
  int NoMatrix;
  int row;
  int column;
  IntVector indices;
  DoubleVector coefficients;
  int error;
  int NoAreas;
  friend CommentStream& operator >> (CommentStream& infile, VariableInfo& varinfo);
  friend ostream& operator << (ostream& out, VariableInfo& varinfo);
};

#endif
