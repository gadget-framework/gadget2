#ifndef popratiomatrix_h
#define popratiomatrix_h

#include "intvector.h"
#include "popratiovector.h"

/**
 * \class popratiomatrix
 * \brief This class implements a dynamic vector of popratiovector values
 */
class popratiomatrix {
public:
  /**
   * \brief This is the default popratiomatrix constructor
   */
  popratiomatrix() { nrow = 0; v = 0; };
  /**
   * \brief This is the popratiovector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of each row to be created (ie. the size of the popratiovector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  popratiomatrix(int nrow, int ncol);
  /**
   * \brief This is the popratiovector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of each row to be created (ie. the size of the popratiovector to be created for each row)
   * \param initial this is the initial value for all the entries of the vector
   */
  popratiomatrix(int nrow, int ncol, popratio initial);
  /**
   * \brief This is the popratiovector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of the rows to be created (ie. the size of the popratiovector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  popratiomatrix(int nrow, const intvector& ncol);
  /**
   * \brief This is the popratiovector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of the rows to be created (ie. the size of the popratiovector to be created for each row)
   * \param initial this is the initial value for all the entries of the vector
   */
  popratiomatrix(int nrow, const intvector& ncol, popratio initial);
  /**
   * \brief This is the popratiomatrix constructor that create a copy of an existing popratiomatrix
   * \param initial this is the popratiomatrix to copy
   */
  popratiomatrix(const popratiomatrix& initial);
  /**
   * \brief This is the popratiomatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~popratiomatrix();
  /**
   * \brief This will return the number of columns in row i of the vector
   * \param i this is the row of the vector to have the number of columns counted
   * \return the number of columns in row i of the vector
   * \note This is the number of entries in the popratiovector that is entry i of the popratiomatrix
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
  popratiovector& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  const popratiovector& operator [] (int pos) const;
  /**
   * \brief This will add new entries to the vector
   * \param add this is the number of new entries to the vector
   * \param length this is the number of entries to the popratiovector that is created
   * \param initial this is the value that will be entered for the new entries
   */
  void AddRows(int add, int length, popratio initial);
  /**
   * \brief This will add new empty entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   * \param add this is the number of new entries to the vector
   * \param length this is the number of entries to the popratiovector that is created
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
   * \brief This is the vector of popratiovector values
   */
  popratiovector** v;
};

#ifdef GADGET_INLINE
#include "popratiomatrix.icc"
#endif

#endif
