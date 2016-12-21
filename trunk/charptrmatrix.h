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
   * \brief This is the CharPtrMatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~CharPtrMatrix();
  /**
   * \brief This will add one new empty entry to the vector
   * \note The new element of the vector will be created, and set to zero
   */
  void resize();
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
