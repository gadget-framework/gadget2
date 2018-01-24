#ifndef formulavector_h
#define formulavector_h

#include "formula.h"

/**
 * \class FormulaVector
 * \brief This class implements a dynamic vector of Formula values
 */
class FormulaVector {
public:
  /**
   * \brief This is the default FormulaVector constructor
   */
  FormulaVector() { size = 0; v = 0; };
  /**
   * \brief This is the FormulaVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  FormulaVector(int sz, Formula initial);
  /**
   * \brief This is the FormulaVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \param initial is the initial value for all the entries of the vector
   */
  FormulaVector(int sz, double initial);
  /**
   * \brief This is the FormulaVector constructor that creates a copy of an existing FormulaVector
   * \param initial is the FormulaVector to copy
   */
  FormulaVector(const FormulaVector& initial);
  /**
   * \brief This is the FormulaVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~FormulaVector();
  /**
   * \brief This will add new blank (ie. NULL) entries to the vector
   * \param addsize is the number of blank entries that will be entered
   * \param keeper is the Keeper for the current model
   * \note The new elements of the vector will be created, and set to zero
   */
  void resize(int addsize, Keeper* keeper);
  /**
   * \brief This will delete an entry from the vector
   * \param pos is the element of the vector to be deleted
   * \param keeper is the Keeper for the current model
   * \note This will free the memory allocated to the deleted element of the vector
   */
  void Delete(int pos, Keeper* keeper);
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
  Formula& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const Formula& operator [] (int pos) const { return v[pos]; };
  /**
   * \brief This function will inform Keeper of the values of the stored Formula
   * \param keeper is the Keeper for the current model
   */
  void Inform(Keeper* keeper);
protected:
  /**
   * \brief This is the size of the vector
   */
  int size;
  /**
   * \brief This is the vector of Formula values
   */
  Formula* v;
};

#endif
