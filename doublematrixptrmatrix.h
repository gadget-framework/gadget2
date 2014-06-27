#ifndef doublematrixptrmatrix_h
#define doublematrixptrmatrix_h

#include "intvector.h"
#include "doublematrixptrvector.h"

class DoubleMatrix;

/**
 * \class DoubleMatrixPtrMatrix
 * \brief This class implements a dynamic vector of DoubleMatrixPtrVector values
 */
class DoubleMatrixPtrMatrix {
public:
  /**
   * \brief This is the default DoubleMatrixPtrMatrix constructor
   */
  DoubleMatrixPtrMatrix() { nrow = 0; v = 0; };
  /**
   * \brief This is the DoubleMatrixPtrMatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~DoubleMatrixPtrMatrix();
  /**
   * \brief This will return the number of columns in row i of the vector
   * \param i is the row of the vector to have the number of columns counted
   * \return the number of columns in row i of the vector
   * \note This is the number of entries in the DoubleMatrixPtrVector that is entry i of the DoubleMatrixPtrMatrix
   */
  int Ncol(int i = 0) const { return v[i]->Size(); };
  /**
   * \brief This will return the number of rows of the vector
   * \return the number of rows of the vector
   */
  int Nrow() const { return nrow; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  DoubleMatrixPtrVector& operator [] (int pos) { return *v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const DoubleMatrixPtrVector& operator [] (int pos) const { return *v[pos]; };
  /**
   * \brief This will add one new empty entry to the vector
   * \note The new element of the vector will be created, and set to zero
   */
  void resize();
  /**
   * \brief This will delete an entry from the vector
   * \param pos is the element of the vector to be deleted
   * \note This will free the memory allocated to the deleted element of the vector
   */
  void Delete(int pos);
protected:
  /**
   * \brief This is number of rows of the vector
   */
  int nrow;
  /**
   * \brief This is the vector of DoubleMatrixPtrVector values
   */
  DoubleMatrixPtrVector** v;
};

#endif
