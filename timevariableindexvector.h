#ifndef timevariableindexvector_h
#define timevariableindexvector_h

#include "timevariable.h"

/**
 * \class TimeVariableIndexVector
 * \brief This class implements a dynamic vector of TimeVariable values, indexed from minpos not 0
 */
class TimeVariableIndexVector {
public:
  /**
   * \brief This is the default TimeVariableIndexVector constructor
   */
  TimeVariableIndexVector() { size = 0; minpos = 0; v = 0; };
  /**
   * \brief This is the TimeVariableIndexVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \param minpos is the index for the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  TimeVariableIndexVector(int sz, int minpos);
  /**
   * \brief This is the TimeVariableIndexVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~TimeVariableIndexVector();
  void resize(int size, int newminpos, Keeper* const keeper);
  void resize(const TimeVariable& tvar, int newminpos, Keeper* const keeper);
  /**
   * \brief This will return the size of the vector
   * \return the size of the vector
   */
  int Size() const { return size; };
  void Read(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  TimeVariable& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const TimeVariable& operator [] (int pos) const;
  int DidChange(const TimeClass* const TimeInfo) const;
  void Update(const TimeClass* const TimeInfo);
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
protected:
  /**
   * \brief This is index for the vector
   */
  int minpos;
  /**
   * \brief This is the size of the vector
   */
  int size;
  /**
   * \brief This is the indexed vector of TimeVariable values
   */
  TimeVariable* v;
};

#ifdef GADGET_INLINE
#include "timevariableindexvector.icc"
#endif

#endif
