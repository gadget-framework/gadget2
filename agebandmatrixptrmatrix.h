#ifndef agebandmatrixptrmatrix_h
#define agebandmatrixptrmatrix_h

#include "agebandmatrixptrvector.h"

/**
 * \class AgeBandMatrixPtrMatrix
 * \brief This class implements a dynamic vector of AgeBandMatrixPtrVector values
 */
class AgeBandMatrixPtrMatrix {
public:
  /**
   * \brief This is the default AgeBandMatrixPtrMatrix constructor
   */
  AgeBandMatrixPtrMatrix() { nrow = 0; v = 0; };
  /**
   * \brief This is the AgeBandMatrixPtrMatrix constructor for a specified size
   * \param nr is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  AgeBandMatrixPtrMatrix(int nr);
  /**
   * \brief This is the AgeBandMatrixPtrMatrix constructor for a specified size with an initial value
   * \param nr is the size of the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  AgeBandMatrixPtrMatrix(int nr, AgeBandMatrixPtrVector* initial);
  /**
   * \brief This is the AgeBandMatrixPtrMatrix constructor that creates a copy of an existing AgeBandMatrixPtrMatrix
   * \param initial is the AgeBandMatrixPtrMatrix to copy
   */
  AgeBandMatrixPtrMatrix(const AgeBandMatrixPtrMatrix& initial);
  /**
   * \brief This is the AgeBandMatrixPtrMatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~AgeBandMatrixPtrMatrix();
  /**
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param value is the value that will be entered for the new entries
   */
  void AddRows(int add, AgeBandMatrixPtrVector* value);
  /**
   * \brief This will add new empty entries to the vector
   * \param add is the number of new entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   */
  void AddRows(int add);
  /**
   * \brief This will delete an entry from the vector
   * \param row is the element of the vector to be deleted
   * \note This will free the memory allocated to the deleted element of the vector
   */
  void DeleteRow(int row);
  /**
   * \brief This will return the number of columns in row i of the vector
   * \param i is the row of the vector to have the number of columns counted
   * \return the number of columns in row i of the vector
   * \note This is the number of entries in the AgeBandMatrixPtrVector that is entry i of the AgeBandMatrixPtrMatrix
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
  AgeBandMatrixPtrVector*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  AgeBandMatrixPtrVector* const& operator [] (int pos) const { return v[pos]; };
protected:
  /**
   * \brief This is number of rows of the vector
   */
  int nrow;
  /**
   * \brief This is the vector of AgeBandMatrixPtrVector values
   */
  AgeBandMatrixPtrVector** v;
};

#endif
