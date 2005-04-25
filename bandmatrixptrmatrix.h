#ifndef bandmatrixptrmatrix_h
#define bandmatrixptrmatrix_h

#include "bandmatrixptrvector.h"

/**
 * \class BandMatrixPtrMatrix
 * \brief This class implements a dynamic vector of BandMatrixPtrVector values
 */
class BandMatrixPtrMatrix {
public:
  /**
   * \brief This is the default BandMatrixPtrMatrix constructor
   */
  BandMatrixPtrMatrix() { nrow = 0; v = 0; };
  /**
   * \brief This is the BandMatrixPtrMatrix constructor for a specified size
   * \param nrow is the size of the vector to be created
   * \param ncol is the length of each row to be created (ie. the size of the BandMatrixPtrVector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  BandMatrixPtrMatrix(int nrow, int ncol);
  /**
   * \brief This is the BandMatrixPtrMatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~BandMatrixPtrMatrix();
  void changeElement(int nrow, int ncol, const BandMatrix& value);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  BandMatrixPtrVector& operator [] (int pos) { return *v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const BandMatrixPtrVector& operator [] (int pos) const { return *v[pos]; };
  /**
   * \brief This will add new empty entries to the vector
   * \param addrow is the number of new entries to the vector
   * \param ncol is the number of entries to the BandMatrixPtrVector that is created
   * \note The new elements of the vector will be created, and set to zero
   */
  void AddRows(int addrow, int ncol);
  /**
   * \brief This will delete an entry from the vector
   * \param row is the element of the vector to be deleted
   * \note This will free the memory allocated to the deleted element of the vector
   */
  void DeleteRow(int row);
  /**
   * \brief This will return the number of rows of the vector
   * \return the number of rows of the vector
   */
  int Nrow() const { return nrow; };
  /**
   * \brief This will return the number of columns in row i of the vector
   * \param i is the row of the vector to have the number of columns counted
   * \return the number of columns in row i of the vector
   * \note This is the number of entries in the BandMatrixPtrVector that is entry i of the BandMatrixPtrMatrix
   */
  int Ncol(int i = 0) const { return v[i]->Size(); };
protected:
  /**
   * \brief This is number of rows of the vector
   */
  int nrow;
  /**
   * \brief This is the vector of BandMatrixPtrVector values
   */
  BandMatrixPtrVector** v;
};

#endif
