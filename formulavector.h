#ifndef formulavector_h
#define formulavector_h

#include "formula.h"

/**
 * \class Formulavector
 * \brief This class implements a dynamic vector of Formula values
 */
class Formulavector {
public:
  /**
   * \brief This is the default Formulavector constructor
   */
  Formulavector() { size = 0; v = 0; };
  /**
   * \brief This is the Formulavector constructor for a specified size
   * \param sz this is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  Formulavector(int sz);
  /**
   * \brief This is the Formulavector constructor that create a copy of an existing Formulavector
   * \param initial this is the Formulavector to copy
   */
  Formulavector(const Formulavector& initial);
  /**
   * \brief This is the Formulavector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~Formulavector();
  void resize(int, Keeper*);
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
  Formula& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  const Formula& operator [] (int pos) const;
  void Inform(Keeper*);
  friend CommentStream& operator >> (CommentStream&, Formulavector&);
protected:
  /**
   * \brief This is size of the vector
   */
  int size;
  /**
   * \brief This is the vector of Formula values
   */
  Formula* v;
};

#ifdef GADGET_INLINE
#include "formulavector.icc"
#endif

#endif
