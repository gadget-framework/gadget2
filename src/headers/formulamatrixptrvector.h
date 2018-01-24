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
   * \brief This will add one new entry to the vector
   * \param value is the value that will be entered for the new entry
   */
  void resize(FormulaMatrix* value);
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
  FormulaMatrix*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  FormulaMatrix* const& operator [] (int pos) const { return v[pos]; };
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

#endif

