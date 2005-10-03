#ifndef popratioindexvector_h
#define popratioindexvector_h

#include "popratiovector.h"

/**
 * \class PopRatioIndexVector
 * \brief This class implements a dynamic vector of PopRatioVector values, indexed from minpos not 0
 */
class PopRatioIndexVector {
public:
  /**
   * \brief This is the default PopRatioIndexVector constructor
   */
  PopRatioIndexVector() { minpos = 0; size = 0; v = 0; };
  /**
   * \brief This is the PopRatioIndexVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \param minpos is the index for the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  PopRatioIndexVector(int sz, int minpos);
  /**
   * \brief This is the PopRatioIndexVector constructor for a specified size with an initial value
   * \param sz is the size of the vector to be created
   * \param minpos is the index for the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  PopRatioIndexVector(int sz, int minpos, const PopRatioVector& initial);
  /**
   * \brief This is the PopRatioIndexVector constructor that creates a copy of an existing PopRatioIndexVector
   * \param initial is the PopRatioIndexVector to copy
   */
  PopRatioIndexVector(const PopRatioIndexVector& initial);
  /**
   * \brief This is the PopRatioIndexVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~PopRatioIndexVector();
  /**
   * \brief This will add new entries to the vector
   * \param addsize is the number of new entries to the vector
   * \param newminpos is the index for the vector to be created
   * \param initial is the value that will be entered for the new entries
   */
  void resize(int addsize, int newminpos, const PopRatioVector& initial);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  PopRatioVector& operator [] (int pos) { return *v[pos - minpos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const PopRatioVector& operator [] (int pos) const { return *v[pos - minpos]; };
  /**
   * \brief This will return the index of the vector
   * \return the index of the vector
   */
  int minCol() const { return minpos; };
  /**
   * \brief This will return the length of the vector
   * \return the length of the vector
   */
  int maxCol() const { return minpos + size; };
  /**
   * \brief This will return the size of the vector
   * \return the size of the vector
   */
  int Size() const { return size; };
protected:
  /**
   * \brief This is the index for the vector
   */
  int minpos;
  /**
   * \brief This is the size of the vector
   */
  int size;
  /**
   * \brief This is the indexed vector of PopRatioVector values
   */
  PopRatioVector** v;
};

#endif
