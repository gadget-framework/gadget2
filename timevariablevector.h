#ifndef timevariablevector_h
#define timevariablevector_h

#include "timevariable.h"

/**
 * \class TimeVariablevector
 * \brief This class implements a dynamic vector of TimeVariable values
 */
class TimeVariablevector {
public:
  /**
   * \brief This is the default TimeVariablevector constructor
   */
  TimeVariablevector() { size = 0; v = 0; };
  /**
   * \brief This is the TimeVariablevector constructor for a specified size
   * \param sz this is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  TimeVariablevector(int sz);
  /**
   * \brief This is the TimeVariablevector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~TimeVariablevector();
  /**
   * \brief This will add new empty entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   * \param size this is the number of new entries to the vector
   */
  void resize(int size);
  void resize(int size, Keeper* const keeper);
  void resize(const TimeVariable& tvar, Keeper* const keeper);
  /**
   * \brief This will return the size of the vector
   * \return the size of the vector
   */
  int Size() const { return size; };
  void Read(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  TimeVariable& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos this is the element of the vector to be returned
   * \return the value of the specified element
   */
  const TimeVariable& operator [] (int pos) const;
  int DidChange(const TimeClass* const TimeInfo) const;
  void Update(const TimeClass* const TimeInfo);
protected:
  /**
   * \brief This is size of the vector
   */
  int size;
  /**
   * \brief This is the vector of TimeVariable values
   */
  TimeVariable* v;
};

#ifdef GADGET_INLINE
#include "timevariablevector.icc"
#endif

#endif
