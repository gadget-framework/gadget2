#ifndef addr_keepmatrix_h
#define addr_keepmatrix_h

#include "intvector.h"
#include "addr_keepvector.h"

/**
 * \class addr_keepmatrix
 * \brief This class implements a dynamic vector of addr_keepvector values
 */
class addr_keepmatrix {
public:
  /**
   * \brief This is the default addr_keepmatrix constructor
   */
  addr_keepmatrix() { nrow = 0; v = 0; };
  /**
   * \brief This is the addr_keepvector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of each row to be created (ie. the size of the addr_keepvector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  addr_keepmatrix(int nrow, int ncol);
  /**
   * \brief This is the addr_keepvector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of each row to be created (ie. the size of the addr_keepvector to be created for each row)
   * \param initial this is the initial value for all the entries of the vector
   */
  addr_keepmatrix(int nrow, int ncol, addr_keep initial);
  /**
   * \brief This is the addr_keepvector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of the rows to be created (ie. the size of the addr_keepvector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  addr_keepmatrix(int nrow, const intvector& ncol);
  /**
   * \brief This is the addr_keepvector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of the rows to be created (ie. the size of the addr_keepvector to be created for each row)
   * \param initial this is the initial value for all the entries of the vector
   */
  addr_keepmatrix(int nrow, const intvector& ncol, addr_keep initial);
  /**
   * \brief This is the addr_keepmatrix constructor that create a copy of an existing addr_keepmatrix
   * \param initial this is the addr_keepmatrix to copy
   */
  addr_keepmatrix(const addr_keepmatrix& initial);
  /**
   * \brief This is the addr_keepmatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~addr_keepmatrix();
  /**
   * \brief This will return the number of columns in row i of the vector
   * \param i this is the row of the vector to have the number of columns counted
   * \return the number of columns in row i of the vector
   * \note This is the number of entries in the addr_keepvector that is entry i of the addr_keepmatrix
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
  addr_keepvector& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  const addr_keepvector& operator [] (int pos) const;
  /**
   * \brief This will add new entries to the vector
   * \param add this is the number of new entries to the vector
   * \param length this is the number of entries to the addr_keepvector that is created
   * \param initial this is the value that will be entered for the new entries
   */
  void AddRows(int add, int length, addr_keep initial);
  /**
   * \brief This will add new empty entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   * \param add this is the number of new entries to the vector
   * \param length this is the number of entries to the addr_keepvector that is created
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
   * \brief This is the vector of addr_keepvector values
   */
  addr_keepvector** v;
};

#ifdef GADGET_INLINE
#include "addr_keepmatrix.icc"
#endif

#endif
