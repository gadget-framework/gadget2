#ifndef bandmatrix_h
#define bandmatrix_h

#include "doublematrix.h"
#include "doubleindexvector.h"

class BandMatrix {
public:
  BandMatrix(const BandMatrix& initial);
  BandMatrix() { minage = 0; nrow = 0; v = 0; };
  BandMatrix(const IntVector& minl, const IntVector& size,
    int minAge = 0, double initial = 0.0);
  BandMatrix(const DoubleMatrix& initial, int minAge = 0, int minl = 0);
  BandMatrix(const DoubleIndexVector& initial, int age);
  BandMatrix(int minl, int lengthsize, int minAge,
    int nrow, double initial = 0.0);
  ~BandMatrix();
  int Nrow() const { return nrow; };
  DoubleIndexVector& operator [] (int row);
  const DoubleIndexVector& operator [] (int row) const;
  BandMatrix& operator += (BandMatrix& b);
  int Ncol(int row) const { return (operator[](row).Size()); };
  int Ncol() const { return (operator[](minage).Size()); };
  int minRow() const { return minage; };
  int maxRow() const { return minage + nrow - 1; };
  int minCol(int row) const { return (operator[](row).minCol()); };
  int maxCol(int row) const { return (operator[](row).maxCol()); };
  int minAge() const { return minage; };
  int maxAge() const { return minage + nrow - 1; };
  int minLength(int age) const { return (operator[](age).minCol()); };
  int maxLength(int age) const { return (operator[](age).maxCol()); };
  void sumColumns(DoubleVector& Result) const;
  void Print(ofstream& outfile) const;
protected:
  DoubleIndexVector** v;
  int nrow;
  int minage;
};

/**
 * \class BandMatrixVector
 * \brief This class implements a dynamic vector of BandMatrix values
 */
class BandMatrixVector {
public:
  /**
   * \brief This is the default BandMatrixVector constructor
   */
  BandMatrixVector() { size = 0; v = 0; };
  /**
   * \brief This is the BandMatrixVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  BandMatrixVector(int sz);
  /**
   * \brief This is the BandMatrixVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~BandMatrixVector();
  void ChangeElement(int nr, const BandMatrix& value);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  BandMatrix& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const BandMatrix& operator [] (int pos) const;
  /**
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param initial is the value that will be entered for the new entries
   */
  void resize(int add, const BandMatrix& initial);
  /**
   * \brief This will add new empty entries to the vector
   * \param add is the number of new entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   */
  void resize(int add);
  /**
   * \brief This will return the size of the vector
   * \return the size of the vector
   */
  int Size() const { return size; };
  /**
   * \brief This will delete an entry from the vector
   * \param pos is the element of the vector to be deleted
   * \note This will free the memory allocated to the deleted element of the vector
   */
  void Delete(int pos);
protected:
  /**
   * \brief This is the size of the vector
   */
  int size;
  /**
   * \brief This is the vector of BandMatrix values
   */
  BandMatrix** v;
};

/**
 * \class BandMatrixMatrix
 * \brief This class implements a dynamic vector of BandMatrixVector values
 */
class BandMatrixMatrix {
public:
  /**
   * \brief This is the default BandMatrixMatrix constructor
   */
  BandMatrixMatrix() { nrow = 0; v = 0; };
  /**
   * \brief This is the BandMatrixMatrix constructor for a specified size
   * \param nrow is the size of the vector to be created
   * \param ncol is the length of each row to be created (ie. the size of the BandMatrixVector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  BandMatrixMatrix(int nrow, int ncol);
  /**
   * \brief This is the BandMatrixMatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~BandMatrixMatrix();
  void ChangeElement(int nrow, int ncol, const BandMatrix& value);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  BandMatrixVector& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const BandMatrixVector& operator [] (int pos) const;
  /**
   * \brief This will add new empty entries to the vector
   * \param addrow is the number of new entries to the vector
   * \param ncol is the number of entries to the BandMatrixVector that is created
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
   * \note This is the number of entries in the BandMatrixVector that is entry i of the BandMatrixMatrix
   */
  int Ncol(int i = 0) const { return v[i]->Size(); };
protected:
  /**
   * \brief This is number of rows of the vector
   */
  int nrow;
  /**
   * \brief This is the vector of BandMatrixVector values
   */
  BandMatrixVector** v;
};

#ifdef GADGET_INLINE
#include "bandmatrix.icc"
#endif

#endif
