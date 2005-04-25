#ifndef bandmatrixptrvector_h
#define bandmatrixptrvector_h

#include "bandmatrix.h"

/**
 * \class BandMatrixPtrVector
 * \brief This class implements a dynamic vector of BandMatrix values
 */
class BandMatrixPtrVector {
public:
  /**
   * \brief This is the default BandMatrixPtrVector constructor
   */
  BandMatrixPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the BandMatrixPtrVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  BandMatrixPtrVector(int sz);
  /**
   * \brief This is the BandMatrixPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~BandMatrixPtrVector();
  void changeElement(int nr, const BandMatrix& value);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  BandMatrix& operator [] (int pos) { return *v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const BandMatrix& operator [] (int pos) const { return *v[pos]; };
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

#endif
