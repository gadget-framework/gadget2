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
  /**
   * \brief This will add new empty entries to the vector and inform Keeper of the change
   * \param addsize is the number of new entries to the vector
   * \param lower is the new index for the vector
   * \param keeper is the Keeper for the current model
   * \note The new elements of the vector will be created, and set to zero
   */
  void resize(int addsize, int lower, Keeper* const keeper);
  /**
   * \brief This will add one new entry to the vector and inform Keeper of the change
   * \param tvar is the new TimeVariable entry to the vector
   * \param lower is the new index for the vector
   * \param keeper is the Keeper for the current model
   */
  void resize(const TimeVariable& tvar, int lower, Keeper* const keeper);
  /**
   * \brief This will return the size of the vector
   * \return the size of the vector
   */
  int Size() const { return size; };
  /**
   * \brief This function will read the value of the TimeVariable entries from file
   * \param infile is the CommentStream to read the parameters from
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  void read(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  TimeVariable& operator [] (int pos) {
    assert(minpos <= pos && pos < minpos + size);
    return v[pos - minpos];
  };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const TimeVariable& operator [] (int pos) const {
    assert(minpos <= pos && pos < minpos + size);
    return v[pos - minpos];
  };
  /**
   * \brief This function will check to see if the TimeVariable values have changed
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the values have changed, 0 otherwise
   */
  int DidChange(const TimeClass* const TimeInfo) const;
  /**
   * \brief This function will update the TimeVariable values
   * \param TimeInfo is the TimeClass for the current model
   */
  void Update(const TimeClass* const TimeInfo);
  /**
   * \brief This will return the index of the vector
   * \return the index of the vector
   */
  int minCol() const { return minpos; };
  /**
   * \brief This will return the length of the vector
   * \return the length of the vector
   */
  int maxCol() const { return minpos + size; };
protected:
  /**
   * \brief This is the index for the vector
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

#endif
