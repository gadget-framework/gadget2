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
   * \brief This is the ParameterVector constructor for a specified size with an initial value
   * \param sz is the size of the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  ParameterVector(int sz, Parameter& initial);
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
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param value is the value that will be entered for the new entries
   */
  void resize(int add, Parameter& value);
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
  ParameterVector& operator = (const ParameterVector& paramv);
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
  friend CommentStream& operator >> (CommentStream& infile, ParameterVector& paramVec);
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

int readVectorInLine(CommentStream& infile, ParameterVector& Vec);

#endif
