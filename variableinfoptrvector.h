#ifndef variableinfoptrvector_h
#define variableinfoptrvector_h

class VariableInfo;

/**
 * \class VariableInfoPtrVector
 * \brief This class implements a dynamic vector of VariableInfo values
 */
class VariableInfoPtrVector {
public:
  /**
   * \brief This is the default VariableInfoPtrVector constructor
   */
  VariableInfoPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the VariableInfoPtrVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  VariableInfoPtrVector(int sz);
  /**
   * \brief This is the VariableInfoPtrVector constructor for a specified size with an initial value
   * \param sz is the size of the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  VariableInfoPtrVector(int sz, VariableInfo* initial);
  /**
   * \brief This is the VariableInfoPtrVector constructor that creates a copy of an existing VariableInfoPtrVector
   * \param initial is the VariableInfoPtrVector to copy
   */
  VariableInfoPtrVector(const VariableInfoPtrVector& initial);
  /**
   * \brief This is the VariableInfoPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~VariableInfoPtrVector();
  /**
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param value is the value that will be entered for the new entries
   */
  void resize(int add, VariableInfo* value);
  /**
   * \brief This will add new empty entries to the vector
   * \param add is the number of new entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   */
  void resize(int add);
  /**
   * \brief This will delete an entry from the vector
   * \param pos is the element of the vector to be deleted
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
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  VariableInfo*& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  VariableInfo* const& operator [] (int pos) const;
protected:
  /**
   * \brief This is the vector of VariableInfo values
   */
  VariableInfo** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#ifdef GADGET_INLINE
#include "variableinfoptrvector.icc"
#endif

#endif
