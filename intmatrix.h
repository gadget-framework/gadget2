#ifndef intmatrix_h
#define intmatrix_h

#include "intvector.h"

/**
 * \class intmatrix
 * \brief This class implements a dynamic vector of intvector values
 */
class intmatrix {
public:
  /**
   * \brief This is the default intmatrix constructor
   */
  intmatrix() { nrow = 0; v = 0; };
  /**
   * \brief This is the intvector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of each row to be created (ie. the size of the intvector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  intmatrix(int nrow, int ncol);
  /**
   * \brief This is the intvector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of each row to be created (ie. the size of the intvector to be created for each row)
   * \param initial this is the initial value for all the entries of the vector
   */
  intmatrix(int nrow, int ncol, int initial);
  /**
   * \brief This is the intvector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of the rows to be created (ie. the size of the intvector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  intmatrix(int nrow, const intvector& ncol);
  /**
   * \brief This is the intvector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of the rows to be created (ie. the size of the intvector to be created for each row)
   * \param initial this is the initial value for all the entries of the vector
   */
  intmatrix(int nrow, const intvector& ncol, int initial);
  /**
   * \brief This is the intmatrix constructor that create a copy of an existing intmatrix
   * \param initial this is the intmatrix to copy
   */
  intmatrix(const intmatrix& initial);
  /**
   * \brief This is the intmatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~intmatrix();
  /**
   * \brief This will return the number of columns in row i of the vector
   * \param i this is the row of the vector to have the number of columns counted
   * \return the number of columns in row i of the vector
   * \note This is the number of entries in the intvector that is entry i of the intmatrix
   */
  int Ncol(int i = 0) const { return v[i]->Size(); };
  /**
   * \brief This will return the number of rows of the vector
   * \return the number of rows of the vector
   */
  int Nrow() const { return nrow; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  intvector& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  const intvector& operator [] (int pos) const;
  /**
   * \brief This will add new entries to the vector
   * \param add this is the number of new entries to the vector
   * \param length this is the number of entries to the intvector that is created
   * \param initial this is the value that will be entered for the new entries
   */
  void AddRows(int add, int length, int initial);
  /**
   * \brief This will add new empty entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   * \param add this is the number of new entries to the vector
   * \param length this is the number of entries to the intvector that is created
   */
  void AddRows(int add, int length);
  /**
   * \brief This will delete an entry from the vector
   * \param row this is the element of the vector to be deleted
   * \note This will free the memory allocated to the deleted element of the vector
   */
  void DeleteRow(int row);
protected:
  /**
   * \brief This is number of rows of the vector
   */
  int nrow;
  /**
   * \brief This is the vector of intvector values
   */
  intvector** v;
};

#ifdef GADGET_INLINE
#include "intmatrix.icc"
#endif

#endif
