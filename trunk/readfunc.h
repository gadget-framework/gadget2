#ifndef readfunc_h
#define readfunc_h

#include "doublematrix.h"
#include "areatime.h"
#include "charptrvector.h"
#include "formulamatrixptrvector.h"
#include "commentstream.h"

/**
 * \brief This is the function that will count the number of columns of data in a file
 * \param infile is the CommentStream to read the data from
 * \return number of columns of data in the file
 * \note This function currently counts the number of columns that the first line of data in the file has, and then assumes that the rest of the file has the same number of columns
 */
int countColumns(CommentStream& infile);
/**
 * \brief This is the function that will read reference weight data from a file into a matrix
 * \param infile is the CommentStream to read the data from
 * \param M is the DoubleMatrix that the reference weight data will be stored in
 */
void readRefWeights(CommentStream& infile, DoubleMatrix& M);
void readAmounts(CommentStream& infile, const IntVector& tmpareas,
  const TimeClass* const TimeInfo, const AreaClass* const Area,
  FormulaMatrix& amount, const char* givenname);
void readGrowthAmounts(CommentStream& infile, const TimeClass* const TimeInfo,
  const AreaClass* const Area, FormulaMatrixPtrVector& amount,
  const CharPtrVector& lenindex, const IntVector& tmpareas);

#endif



