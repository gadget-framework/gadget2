#ifndef suitfuncptrvector_h
#define suitfuncptrvector_h

#include "suitfunc.h"

class SuitFunc;

/**
 * \class SuitfuncPtrvector
 * \brief This class implements a dynamic vector of SuitFunc values
 */
class SuitfuncPtrvector {
public:
  /**
   * \brief This is the default SuitfuncPtrvector constructor
   */
  SuitfuncPtrvector() { size = 0; v = 0; };
  /**
   * \brief This is the SuitfuncPtrvector constructor for a specified size
   * \param sz this is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  SuitfuncPtrvector(int sz);
  /**
   * \brief This is the SuitfuncPtrvector constructor for a specified size with an initial value
   * \param sz this is the size of the vector to be created
   * \param initial this is the initial value for all the entries of the vector
   */
  SuitfuncPtrvector(int sz, SuitFunc* initial);
  /**
   * \brief This is the SuitfuncPtrvector constructor that create a copy of an existing SuitfuncPtrvector
   * \param initial this is the SuitfuncPtrvector to copy
   */
  SuitfuncPtrvector(const SuitfuncPtrvector& initial);
  /**
   * \brief This is the SuitfuncPtrvector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~SuitfuncPtrvector();
  /**
   * \brief This will add new entries to the vector
   * \param add this is the number of new entries to the vector
   * \param suitf this is the value that will be entered for the new entries
   */
  void resize(int add, SuitFunc* suitf);
  /**
   * \brief This will add new empty entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   * \param add this is the number of new entries to the vector
   */
  void resize(int add);
  /**
   * \brief This will delete an entry from the vector
   * \param pos this is the element of the vector to be deleted
   * \param keeper this is the Keeper class for the model
   * \note This will free the memory allocated to the deleted element of the vector
   */
  void Delete(int pos, Keeper* const keeper);
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
  SuitFunc*& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  SuitFunc* const& operator [] (int pos) const;
protected:
  /**
   * \brief This is the vector of SuitFunc values
   */
  SuitFunc** v;
  /**
   * \brief This is size of the vector
   */
  int size;
};

#ifdef GADGET_INLINE
#include "suitfuncptrvector.icc"
#endif

#endif
