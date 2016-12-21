#ifndef readaggregation_h
#define readaggregation_h

#include "commentstream.h"
#include "intmatrix.h"
#include "doublematrix.h"
#include "charptrmatrix.h"
#include "formulamatrix.h"

/**
 * \brief This is the function that will read aggregation data from a file into a matrix
 * \param infile is the CommentStream to read the aggregation data from
 * \param agg is the IntMatrix that the aggregated data will be stored in
 * \param aggindex is the CharPtrVector that the aggregated data labels will be stored in
 * \return number of aggregation entries read
 */
int readAggregation(CommentStream& infile, IntMatrix& agg, CharPtrVector& aggindex);
/**
 * \brief This is the function that will read aggregation data from a file into a vector
 * \param infile is the CommentStream to read the aggregation data from
 * \param agg is the IntVector that the aggregated data will be stored in
 * \param aggindex is the CharPtrVector that the aggregated data labels will be stored in
 * \return number of aggregation entries read
 */
int readAggregation(CommentStream& infile, IntVector& agg, CharPtrVector& aggindex);
/**
 * \brief This is the function that will read length aggregation data from a file
 * \param infile is the CommentStream to read the aggregation data from
 * \param lengths is the DoubleVector that the aggregated length data will be stored in
 * \param lenindex is the CharPtrVector that the aggregated length labels will be stored in
 * \return number of length aggregation entries read
 */
int readLengthAggregation(CommentStream& infile, DoubleVector& lengths, CharPtrVector& lenindex);
/**
 * \brief This is the function that will read prey aggregation data from a file
 * \param infile is the CommentStream to read the aggregation data from
 * \param preynames is the CharPtrVector that the prey names will be stored in
 * \param preylengths is the DoubleMatrix that the aggregated prey length data will be stored in
 * \param digestioncoeff is the FormulaMatrix that the prey digestion data will be stored in
 * \param preyindex is the CharPtrVector that the aggregated prey labels will be stored in
 * \param keeper is the Keeper for the current model
 * \return number of prey aggregation entries read
 */
int readPreyAggregation(CommentStream& infile, CharPtrMatrix& preynames, DoubleMatrix& preylengths,
  FormulaMatrix& digestioncoeff, CharPtrVector& preyindex, Keeper* const keeper);

#endif
