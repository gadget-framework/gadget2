#ifndef charptrmatrix_h
#define charptrmatrix_h

#include "intvector.h"
#include "charptrvector.h"

/**
 * \class CharPtrMatrix
 * \brief This class implements a dynamic vector of CharPtrVector values
 */
class CharPtrMatrix {
public:
  /**
   * \brief This is the default CharPtrMatrix constructor
   */
  CharPtrMatrix() { nrow = 0; v = 0; };
  /**
   * \brief This is the CharPtrVector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of each row to be created (ie. the size of the CharPtrVector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  CharPtrMatrix(int nrow, int ncol);
  /**
   * \brief This is the CharPtrVector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of each row to be created (ie. the size of the CharPtrVector to be created for each row)
   * \param initial this is the initial value for all the entries of the vector
   */
  CharPtrMatrix(int nrow, int ncol, char* initial);
  /**
   * \brief This is the CharPtrVector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of the rows to be created (ie. the size of the CharPtrVector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  CharPtrMatrix(int nrow, const IntVector& ncol);
  /**
   * \brief This is the CharPtrVector constructor for a specified size
   * \param nrow this is the size of the vector to be created
   * \param ncol this is the length of the rows to be created (ie. the size of the CharPtrVector to be created for each row)
   * \param initial this is the initial value for all the entries of the vector
   */
  CharPtrMatrix(int nrow, const IntVector& ncol, char* initial);
  /**
   * \brief This is the CharPtrMatrix constructor that create a copy of an existing CharPtrMatrix
   * \param initial this is the CharPtrMatrix to copy
   */
  CharPtrMatrix(const CharPtrMatrix& initial);
  /**
   * \brief This is the CharPtrMatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~CharPtrMatrix();
  /**
   * \brief This will return the number of columns in row i of the vector
   * \param i this is the row of the vector to have the number of columns counted
   * \return the number of columns in row i of the vector
   * \note This is the number of entries in the CharPtrVector that is entry i of the CharPtrMatrix
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
  CharPtrVector& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  const CharPtrVector& operator [] (int pos) const;
  /**
   * \brief This will add new entries to the vector
   * \param add this is the number of new entries to the vector
   * \param length this is the number of entries to the CharPtrVector that is created
   * \param initial this is the value that will be entered for the new entries
   */
  void AddRows(int add, int length, char* initial);
  /**
   * \brief This will add new empty entries to the vector
   * \param add this is the number of new entries to the vector
   * \param length this is the number of entries to the CharPtrVector that is created
   * \note The new elements of the vector will be created, and set to zero
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
   * \brief This is the vector of CharPtrVector values
   */
  CharPtrVector** v;
};

#ifdef GADGET_INLINE
#include "charptrmatrix.icc"
#endif

#endif
