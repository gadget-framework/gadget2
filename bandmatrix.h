#ifndef bandmatrix_h
#define bandmatrix_h

#include "intvector.h"
#include "doublematrix.h"
#include "doubleindexvector.h"

class bandmatrix {
public:
  bandmatrix(const bandmatrix& initial);
  bandmatrix() { minage = 0; nrow = 0; v = 0; };
  bandmatrix(const intvector& minl, const intvector& size,
    int Minage = 0, double initial = 0);
  bandmatrix(const doublematrix& initial, int Minage = 0, int minl = 0);
  bandmatrix(const doubleindexvector& initial, int age);
  bandmatrix(int minl, int lengthsize, int minage,
    int nrow, double initial = 0);
  ~bandmatrix();
  int Nrow() const { return nrow; };
  doubleindexvector& operator [] (int row);
  const doubleindexvector& operator [] (int row) const;
  bandmatrix& operator += (bandmatrix& b);
  int Ncol(int row) const { return (operator[](row).Size()); };
  int Ncol() const { return (operator[](minage).Size()); };
  int Minrow() const { return minage; };
  int Maxrow() const { return minage + nrow - 1; };
  int Mincol(int row) const { return (operator[](row).Mincol()); };
  int Maxcol(int row) const { return (operator[](row).Maxcol()); };
  int Minage() const { return minage; };
  int Maxage() const { return minage + nrow - 1; };
  int Minlength(int age) const { return (operator[](age).Mincol()); };
  int Maxlength(int age) const { return (operator[](age).Maxcol()); };
  void Colsum(doublevector& Result) const;
protected:
  doubleindexvector** v;
  int nrow;
  int minage;
};

/**
 * \class bandmatrixvector
 * \brief This class implements a dynamic vector of bandmatrix values
 */
class bandmatrixvector {
public:
  /**
   * \brief This is the default bandmatrixvector constructor
   */
  bandmatrixvector() { size = 0; v = 0; };
  /**
   * \brief This is the bandmatrixvector constructor for a specified size
   * \param sz this is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  bandmatrixvector(int sz);
  /**
   * \brief This is the bandmatrixvector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~bandmatrixvector();
  void ChangeElement(int nr, const bandmatrix& value);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  bandmatrix& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  const bandmatrix& operator [] (int pos) const;
  /**
   * \brief This will add new entries to the vector
   * \param add this is the number of new entries to the vector
   * \param initial this is the value that will be entered for the new entries
   */
  void resize(int add, const bandmatrix& initial);
  /**
   * \brief This will add new empty entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   * \param add this is the number of new entries to the vector
   */
  void resize(int add);
  /**
   * \brief This will return the size of the vector
   * \return the size of the vector
   */
  int Size() const { return size; };
  /**
   * \brief This will delete an entry from the vector
   * \param pos this is the element of the vector to be deleted
   * \note This will free the memory allocated to the deleted element of the vector
   */
  void Delete(int pos);
protected:
  /**
   * \brief This is size of the vector
   */
  int size;
  /**
   * \brief This is the vector of bandmatrix values
   */
  bandmatrix** v;
};

/**
 * \class bandmatrixmatrix
 * \brief This class implements a dynamic vector of bandmatrixvector values
 */
class bandmatrixmatrix {
public:
  /**
   * \brief This is the default bandmatrixmatrix constructor
   */
  bandmatrixmatrix() { nrow = 0; v = 0; };
  /**
   * \brief This is the bandmatrixvector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of each row to be created (ie. the size of the bandmatrixvector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  bandmatrixmatrix(int nrow, int ncol);
  /**
   * \brief This is the bandmatrixmatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~bandmatrixmatrix();
  void ChangeElement(int nrow, int ncol, const bandmatrix& value);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  bandmatrixvector& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  const bandmatrixvector& operator [] (int pos) const;
  /**
   * \brief This will add new empty entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   * \param addrow this is the number of new entries to the vector
   * \param ncol this is the number of entries to the bandmatrixvector that is created
   */
  void AddRows(int addrow, int ncol);
  /**
   * \brief This will delete an entry from the vector
   * \param row this is the element of the vector to be deleted
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
   * \param i this is the row of the vector to have the number of columns counted
   * \return the number of columns in row i of the vector
   * \note This is the number of entries in the bandmatrixvector that is entry i of the bandmatrixmatrix
   */
  int Ncol(int i = 0) const { return v[i]->Size(); };
protected:
  /**
   * \brief This is number of rows of the vector
   */
  int nrow;
  /**
   * \brief This is the vector of bandmatrixvector values
   */
  bandmatrixvector** v;
};

#ifdef GADGET_INLINE
#include "bandmatrix.icc"
#endif

#endif
