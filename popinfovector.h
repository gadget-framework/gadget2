#ifndef popinfovector_h
#define popinfovector_h

#include "conversion.h"
#include "popinfo.h"

/**
 * \class popinfovector
 * \brief This class implements a dynamic vector of popinfo values
 */
class popinfovector {
public:
  /**
   * \brief This is the default popinfovector constructor
   */
  popinfovector() { size = 0; v = 0; };
  /**
   * \brief This is the popinfovector constructor for a specified size
   * \param sz this is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  popinfovector(int sz);
  /**
   * \brief This is the popinfovector constructor for a specified size with an initial value
   * \param sz this is the size of the vector to be created
   * \param initial this is the initial value for all the entries of the vector
   */
  popinfovector(int sz, popinfo initial);
  /**
   * \brief This is the popinfovector constructor that create a copy of an existing popinfovector
   * \param initial this is the popinfovector to copy
   */
  popinfovector(const popinfovector& initial);
  /**
   * \brief This is the popinfovector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~popinfovector();
  /**
   * \brief This will add new entries to the vector
   * \param add this is the number of new entries to the vector
   * \param value this is the value that will be entered for the new entries
   */
  void resize(int add, popinfo value);
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
  popinfo& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  const popinfo& operator [] (int pos) const;
  void Sum(const popinfovector* const Number, const ConversionIndex& CI);
protected:
  /**
   * \brief This is the vector of popinfo values
   */
  popinfo* v;
  /**
   * \brief This is size of the vector
   */
  int size;
};

#ifdef GADGET_INLINE
#include "popinfovector.icc"
#endif

#endif
