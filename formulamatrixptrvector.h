#ifndef formulamatrixptrvector_h
#define formulamatrixptrvector_h

#include "formulamatrix.h"

/**
 * \class FormulaMatrixPtrVector
 * \brief This class implements a dynamic vector of FormulaMatrix values
 */
class FormulaMatrixPtrVector {
public:
  /**
   * \brief This is the default FormulaMatrixPtrVector constructor
   */
  FormulaMatrixPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the FormulaMatrixPtrVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  FormulaMatrixPtrVector(int sz);
  /**
   * \brief This is the FormulaMatrixPtrVector constructor for a specified size with an initial value
   * \param sz is the size of the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  FormulaMatrixPtrVector(int sz, FormulaMatrix* initial);
  /**
   * \brief This is the FormulaMatrixPtrVector constructor that creates a copy of an existing FormulaMatrixPtrVector
   * \param initial is the FormulaMatrixPtrVector to copy
   */
  FormulaMatrixPtrVector(const FormulaMatrixPtrVector& initial);
  /**
   * \brief This is the FormulaMatrixPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~FormulaMatrixPtrVector();
  /**
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param value is the value that will be entered for the new entries
   */
  void resize(int add, FormulaMatrix* value);
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
  FormulaMatrix*& operator [] (int pos);
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  FormulaMatrix* const& operator [] (int pos) const;
protected:
  /**
   * \brief This is the vector of FormulaMatrix values
   */
  FormulaMatrix** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#ifdef GADGET_INLINE
#include "formulamatrixptrvector.icc"
#endif

#endif

