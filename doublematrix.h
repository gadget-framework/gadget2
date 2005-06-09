#ifndef doublematrix_h
#define doublematrix_h

#include "doublevector.h"
#include "intvector.h"

/**
 * \class DoubleMatrix
 * \brief This class implements a dynamic vector of DoubleVector values
 */
class DoubleMatrix {
public:
  /**
   * \brief This is the default DoubleMatrix constructor
   */
  DoubleMatrix() { nrow = 0; v = 0; };
  /**
   * \brief This is the DoubleMatrix constructor for a specified size
   * \param nr is the size of the vector to be created
   * \param nc is the length of each row to be created (ie. the size of the DoubleVector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  DoubleMatrix(int nr, int nc);
  /**
   * \brief This is the DoubleMatrix constructor for a specified size
   * \param nr is the size of the vector to be created
   * \param nc is the length of each row to be created (ie. the size of the DoubleVector to be created for each row)
   * \param initial is the initial value for all the entries of the vector
   */
  DoubleMatrix(int nr, int nc, double initial);
  /**
   * \brief This is the DoubleMatrix constructor for a specified size
   * \param nr is the size of the vector to be created
   * \param nc is the length of the rows to be created (ie. the size of the DoubleVector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  DoubleMatrix(int nr, const IntVector& nc);
  /**
   * \brief This is the DoubleMatrix constructor for a specified size
   * \param nr is the size of the vector to be created
   * \param nc is the length of the rows to be created (ie. the size of the DoubleVector to be created for each row)
   * \param initial is the initial value for all the entries of the vector
   */
  DoubleMatrix(int nr, const IntVector& nc, double initial);
  /**
   * \brief This is the DoubleMatrix constructor that creates a copy of an existing DoubleMatrix
   * \param initial is the DoubleMatrix to copy
   */
  DoubleMatrix(const DoubleMatrix& initial);
  /**
   * \brief This is the DoubleMatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~DoubleMatrix();
  /**
   * \brief This will return the number of columns in row i of the vector
   * \param i is the row of the vector to have the number of columns counted
   * \return the number of columns in row i of the vector
   * \note This is the number of entries in the DoubleVector that is entry i of the DoubleMatrix
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
  DoubleVector& operator [] (int pos) { return *v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const DoubleVector& operator [] (int pos) const { return *v[pos]; };
  /**
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param length is the number of entries to the DoubleVector that is created
   * \param initial is the value that will be entered for the new entries
   */
  void AddRows(int add, int length, double initial);
  /**
   * \brief This will add new empty entries to the vector
   * \param add is the number of new entries to the vector
   * \param length is the number of entries to the DoubleVector that is created
   * \note The new elements of the vector will be created, and set to zero
   */
  void AddRows(int add, int length);
  /**
   * \brief This will delete an entry from the vector
   * \param row is the element of the vector to be deleted
   * \note This will free the memory allocated to the deleted element of the vector
   */
  void DeleteRow(int row);
  DoubleMatrix& operator = (const DoubleMatrix& d);
protected:
  /**
   * \brief This is number of rows of the vector
   */
  int nrow;
  /**
   * \brief This is the vector of DoubleVector values
   */
  DoubleVector** v;
};

#endif
