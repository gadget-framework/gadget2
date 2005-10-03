#ifndef doubleindexvector_h
#define doubleindexvector_h

#include "gadget.h"

/**
 * \class DoubleIndexVector
 * \brief This class implements a dynamic vector of double values, indexed from minpos not 0
 */
class DoubleIndexVector {
public:
  /**
   * \brief This is the default DoubleIndexVector constructor
   */
  DoubleIndexVector() { minpos = 0; size = 0; v = 0; };
  /**
   * \brief This is the DoubleIndexVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \param minpos is the index for the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  DoubleIndexVector(int sz, int minpos);
  /**
   * \brief This is the DoubleIndexVector constructor for a specified size with an initial value
   * \param sz is the size of the vector to be created
   * \param minpos is the index for the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  DoubleIndexVector(int sz, int minpos, double initial);
  /**
   * \brief This is the DoubleIndexVector constructor that creates a copy of an existing DoubleIndexVector
   * \param initial is the DoubleIndexVector to copy
   */
  DoubleIndexVector(const DoubleIndexVector& initial);
  /**
   * \brief This is the DoubleIndexVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~DoubleIndexVector();
  /**
   * \brief This will add new entries to the vector
   * \param addsize is the number of new entries to the vector
   * \param newminpos is the index for the vector to be created
   * \param value is the value that will be entered for the new entries
   */
  void resize(int addsize, int newminpos, double value);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  double& operator [] (int pos) { return v[pos - minpos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const double& operator [] (int pos) const { return v[pos - minpos]; };
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
  /**
   * \brief This will delete an element of the vector
   * \param pos is the element of the vector to be deleted
   */
  void Delete(int pos);
  /**
   * \brief This operator will set the vector equal to an existing DoubleIndexVector
   * \param dv is the DoubleIndexVector to copy
   */
  DoubleIndexVector& operator = (const DoubleIndexVector& dv);
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
   * \brief This is the indexed vector of double values
   */
  double* v;
};

#endif
