#ifndef formulaindexvector_h
#define formulaindexvector_h

#include "formula.h"

class Keeper;

/**
 * \class Formulaindexvector
 * \brief This class implements a dynamic vector of Formula values, indexed from minpos not 0
 */
class Formulaindexvector {
public:
  /**
   * \brief This is the default Formulaindexvector constructor
   */
  Formulaindexvector() { size = 0; minpos = 0; v = 0; };
  /**
   * \brief This is the Formulaindexvector constructor for a specified size
   * \param sz this is the size of the vector to be created
   * \param minpos this is the index for the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  Formulaindexvector(int sz, int minpos) ;
  /**
   * \brief This is the Formulaindexvector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~Formulaindexvector();
  void resize(int addsize, int newminpos, Keeper* keeper);
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
  friend CommentStream& operator >> (CommentStream&, Formulaindexvector&);
  void Inform(Keeper* keeper);
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
private:
  /**
   * \brief This is index for the vector
   */
  int minpos;
  /**
   * \brief This is size of the vector
   */
  int size;
  /**
   * \brief This is the indexed vector of Formula values
   */
  Formula* v;
};

#ifdef GADGET_INLINE
#include "formulaindexvector.icc"
#endif

#endif
