#ifndef doublevector_h
#define doublevector_h

#include "mathfunc.h"
#include "gadget.h"

/**
 * \class DoubleVector
 * \brief This class implements a dynamic vector of double values
 */
class DoubleVector {
public:
  /**
   * \brief This is the default DoubleVector constructor
   */
  DoubleVector() { size = 0; v = 0; };
  /**
   * \brief This is the DoubleVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  DoubleVector(int sz);
  /**
   * \brief This is the DoubleVector constructor for a specified size with an initial value
   * \param sz is the size of the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  DoubleVector(int sz, double initial);
  /**
   * \brief This is the DoubleVector constructor that creates a copy of an existing DoubleVector
   * \param initial is the DoubleVector to copy
   */
  DoubleVector(const DoubleVector& initial);
  /**
   * \brief This is the DoubleVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~DoubleVector();
  /**
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param value is the value that will be entered for the new entries
   */
  void resize(int add, double value);
  /**
   * \brief This will add new empty entries to the vector
   * \param add is the number of new entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   */
  void resize(int add);
  /**
   * \brief This will delete an entry from the vector
   * \param pos is the element of the vector to be deleted
   * \note This will free the memory allocated to the deleted element of the vector
   */
  void Delete(int pos);
  /**
   * \brief This will return the size of the vector
   * \return the size of the vector
   */
  int Size() const { return size; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  double& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const double& operator [] (int pos) const { return v[pos]; };
  void Reset();
  double operator * (const DoubleVector& d) const;
  DoubleVector& operator = (const DoubleVector& d);
protected:
  /**
   * \brief This is the vector of double values
   */
  double* v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
