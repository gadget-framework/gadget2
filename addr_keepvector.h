#ifndef addr_keepvector_h
#define addr_keepvector_h

#include "addr_keep.h"

/**
 * \class addr_keepvector
 * \brief This class implements a dynamic vector of addr_keep values
 */
class addr_keepvector {
public:
  /**
   * \brief This is the default addr_keepvector constructor
   */
  addr_keepvector() { size = 0; v = 0; };
  /**
   * \brief This is the addr_keepvector constructor for a specified size
   * \param sz this is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  addr_keepvector(int sz);
  /**
   * \brief This is the addr_keepvector constructor for a specified size with an initial value
   * \param sz this is the size of the vector to be created
   * \param initial this is the initial value for all the entries of the vector
   */
  addr_keepvector(int sz, addr_keep initial);
  /**
   * \brief This is the addr_keepvector constructor that create a copy of an existing addr_keepvector
   * \param initial this is the addr_keepvector to copy
   */
  addr_keepvector(const addr_keepvector& initial);
  /**
   * \brief This is the addr_keepvector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~addr_keepvector();
  /**
   * \brief This will add new entries to the vector
   * \param add this is the number of new entries to the vector
   * \param value this is the value that will be entered for the new entries
   */
  void resize(int add, addr_keep value);
  /**
   * \brief This will add new empty entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   * \param add this is the number of new entries to the vector
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
  addr_keep& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  const addr_keep& operator [] (int pos) const;
protected:
  /**
   * \brief This is the vector of addr_keep values
   */
  addr_keep* v;
  /**
   * \brief This is size of the vector
   */
  int size;
};

#ifdef GADGET_INLINE
#include "addr_keepvector.icc"
#endif

#endif
