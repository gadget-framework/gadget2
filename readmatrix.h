#ifndef readmatrix_h
#define readmatrix_h

#include "commentstream.h"
#include "formulamatrixptrvector.h"

class TimeClass;
class Keeper;

int ReadAmounts(CommentStream& infile, const intvector& tmpareas,
  const TimeClass* const TimeInfo, const AreaClass* const Area,
  Formulamatrix& amount, Keeper* const keeper, const char* givenname);
int ReadGrowthAmounts(CommentStream& infile, const TimeClass* const TimeInfo,
  const AreaClass* const Area, Formulamatrixptrvector& amount,
  const charptrvector& lenindex, Keeper* const keeper);
int ReadContinuousTimeMatrix(CommentStream& infile, int NumberOfColumns,
  const TimeClass* const TimeInfo, Formulamatrix& amount, Keeper* const keeper);

#endif
