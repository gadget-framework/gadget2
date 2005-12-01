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
   * \brief This is the CharPtrMatrix constructor for a specified size
   * \param nr is the size of the vector to be created
   * \param nc is the length of each row to be created (ie. the size of the CharPtrVector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  CharPtrMatrix(int nr, int nc);
  /**
   * \brief This is the CharPtrMatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~CharPtrMatrix();
  /**
   * \brief This will return the number of columns in row i of the vector
   * \param i is the row of the vector to have the number of columns counted
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
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  CharPtrVector& operator [] (int pos) { return *v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const CharPtrVector& operator [] (int pos) const { return *v[pos]; };
  /**
   * \brief This will add new empty entries to the vector
   * \param add is the number of new entries to the vector
   * \param length is the number of entries to the CharPtrVector that is created
   * \note The new elements of the vector will be created, and set to zero
   */
  void AddRows(int add, int length);
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

#endif
