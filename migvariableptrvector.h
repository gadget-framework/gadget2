#ifndef migvariableptrvector_h
#define migvariableptrvector_h

#include "migvariables.h"

/**
 * \class MigVariablePtrVector
 * \brief This class implements a dynamic vector of MigVariable values
 */
class MigVariablePtrVector {
public:
  /**
   * \brief This is the default MigVariablePtrVector constructor
   */
  MigVariablePtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the MigVariablePtrVector constructor for a specified size
   * \param sz this is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  MigVariablePtrVector(int sz);
  /**
   * \brief This is the MigVariablePtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~MigVariablePtrVector();
  /**
   * \brief This will add new empty entries to the vector
   * \param add this is the number of new entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   */
  void resize(int add);
  /**
   * \brief This will delete an entry from the vector
   * \param pos this is the element of the vector to be deleted
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
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  MigVariable*& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  MigVariable* const& operator [] (int pos) const;
protected:
  /**
   * \brief This is the vector of MigVariable values
   */
  MigVariable** v;
  /**
   * \brief This is size of the vector
   */
  int size;
};

#ifdef GADGET_INLINE
#include "migvariableptrvector.icc"
#endif

#endif
