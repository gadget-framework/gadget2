#ifndef popratioindexvector_h
#define popratioindexvector_h

#include "popratiovector.h"

/**
 * \class popratioindexvector
 * \brief This class implements a dynamic vector of popratiovector values, indexed from minpos not 0
 */
class popratioindexvector {
public:
  /**
   * \brief This is the default popratioindexvector constructor
   */
  popratioindexvector() { minpos = 0; size = 0; v = 0; };
  /**
   * \brief This is the popratioindexvector constructor for a specified size
   * \param sz this is the size of the vector to be created
   * \param minpos this is the index for the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  popratioindexvector(int sz, int minpos);
  /**
   * \brief This is the popratioindexvector constructor for a specified size with an initial value
   * \param sz this is the size of the vector to be created
   * \param minpos this is the index for the vector to be created
   * \param initial this is the initial value for all the entries of the vector
   */
  popratioindexvector(int sz, int minpos, const popratiovector& initial);
  /**
   * \brief This is the popratioindexvector constructor that create a copy of an existing popratioindexvector
   * \param initial this is the popratioindexvector to copy
   */
  popratioindexvector(const popratioindexvector& initial);
  /**
   * \brief This is the popratioindexvector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~popratioindexvector();
  /**
   * \brief This will add new entries to the vector
   * \param addsize this is the number of new entries to the vector
   * \param newminpos this is the index for the vector to be created
   * \param initial this is the value that will be entered for the new entries
   */
  void resize(int addsize, int newminpos, const popratiovector& initial);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  popratiovector& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  const popratiovector& operator [] (int pos) const;
  /**
   * \brief This will return the index of the vector
   * \return the index of the vector
   */
  int Mincol() const { return minpos; };
  /**
   * \brief This will return the length of the vector
   * \return the length of the vector
   */
  int Maxcol() const { return minpos + size; };
  /**
   * \brief This will return the size of the vector
   * \return the size of the vector
   */
  int Size() const { return size; };
protected:
  /**
   * \brief This is index for the vector
   */
  int minpos;
  /**
   * \brief This is size of the vector
   */
  int size;
  /**
   * \brief This is the indexed vector of popratiovector values
   */
  popratiovector** v;
};

#ifdef GADGET_INLINE
#include "popratioindexvector.icc"
#endif

#endif
