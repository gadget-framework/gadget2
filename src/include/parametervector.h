#ifndef parametervector_h
#define parametervector_h

#include "commentstream.h"
#include "parameter.h"
#include "gadget.h"

/**
 * \class ParameterVector
 * \brief This class implements a dynamic vector of Parameter values
 */
class ParameterVector {
public:
  /**
   * \brief This is the default ParameterVector constructor
   */
  ParameterVector() { size = 0; v = 0; };
  /**
   * \brief This is the ParameterVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  ParameterVector(int sz);
  /**
   * \brief This is the ParameterVector constructor that creates a copy of an existing ParameterVector
   * \param initial is the ParameterVector to copy
   */
  ParameterVector(const ParameterVector& initial);
  /**
   * \brief This is the ParameterVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~ParameterVector();
  /**
   * \brief This will add one new entry to the vector
   * \param value is the value that will be entered for the new entry
   */
  void resize(Parameter& value);
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
  Parameter& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  Parameter const& operator [] (int pos) const { return v[pos]; };
  /**
   * \brief This operator will set the vector equal to an existing ParameterVector
   * \param pv is the ParameterVector to copy
   */
  ParameterVector& operator = (const ParameterVector& pv);
protected:
  /**
   * \brief This is the vector of Parameter values
   */
  Parameter* v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
