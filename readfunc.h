#ifndef readfunc_h
#define readfunc_h

#include "doublematrix.h"
#include "formulamatrixptrvector.h"
#include "parametervector.h"
#include "commentstream.h"

/**
 * \brief This is the function that will read data from a file into a matrix
 * \param infile is the CommentStream to read the data from
 * \param M is the DoubleMatrix that the data will be stored in
 * \return 1 if the read from file was successful, 0 otherwise
 */
int readMatrix(CommentStream& infile, DoubleMatrix& M);
/**
 * \brief This is the function that will read data from a file into a vector
 * \param infile is the CommentStream to read the data from
 * \param Vec is the IntVector that the data will be stored in
 * \return 1 if the read from file was successful, 0 otherwise
 */
int readVector(CommentStream& infile, IntVector& Vec);
/**
 * \brief This is the function that will read data from a file into a vector
 * \param infile is the CommentStream to read the data from
 * \param Vec is the DoubleVector that the data will be stored in
 * \return 1 if the read from file was successful, 0 otherwise
 */
int readVector(CommentStream& infile, DoubleVector& Vec);
/**
 * \brief This is the function that will read data from a file into an index vector
 * \param infile is the CommentStream to read the data from
 * \param Vec is the DoubleIndexVector that the data will be stored in
 * \return 1 if the read from file was successful, 0 otherwise
 */
int readIndexVector(CommentStream& infile, DoubleIndexVector& Vec);
/**
 * \brief This is the function that will read data from a line from a file into a vector
 * \param infile is the CommentStream to read the data from
 * \param Vec is the IntVector that the data will be stored in
 * \return 1 if the read from file was successful, 0 otherwise
 */
int readVectorInLine(CommentStream& infile, IntVector& Vec);
/**
 * \brief This is the function that will read data from a line from a file into a vector
 * \param infile is the CommentStream to read the data from
 * \param Vec is the DoubleVector that the data will be stored in
 * \return 1 if the read from file was successful, 0 otherwise
 */
int readVectorInLine(CommentStream& infile, DoubleVector& Vec);
/**
 * \brief This is the function that will count the number of columns of data in a file
 * \param infile is the CommentStream to read the data from
 * \return number of columns of data in the file
 * \note this function currently counts the number of columns that the first line of data in the file has, and then assumes that the rest of the file has the same number of columns
 */
int countColumns(CommentStream& infile);
/**
 * \brief This is the function that will read reference weight data from a file into a matrix
 * \param infile is the CommentStream to read the data from
 * \param M is the IntVector that the reference weight data will be stored in
 * \return 1 if the read from file was successful, 0 otherwise
 */
int readRefWeights(CommentStream& infile, DoubleMatrix& M);
int FindContinuousYearAndStepWithNoText(CommentStream& infile, int year, int step);
int readAmounts(CommentStream& infile, const IntVector& tmpareas,
  const TimeClass* const TimeInfo, const AreaClass* const Area,
  FormulaMatrix& amount, const char* givenname);
int readGrowthAmounts(CommentStream& infile, const TimeClass* const TimeInfo,
  const AreaClass* const Area, FormulaMatrixPtrVector& amount,
  const CharPtrVector& lenindex, const IntVector& tmpareas);

#endif



