#ifndef agebandmatrixptrvector_h
#define agebandmatrixptrvector_h

#include "agebandmatrix.h"

class AgeBandMatrix;

/**
 * \class AgeBandMatrixPtrVector
 * \brief This class implements a dynamic vector of AgeBandMatrix values
 */
class AgeBandMatrixPtrVector {
public:
  /**
   * \brief This is the default AgeBandMatrixPtrVector constructor
   */
  AgeBandMatrixPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the AgeBandMatrixPtrVector constructor for a specified size
   * \param sz is the size of the vector to be created
   * \note The elements of the vector will all be created, and set to zero
   */
  AgeBandMatrixPtrVector(int sz);
  /**
   * \brief This is the AgeBandMatrixPtrVector constructor for a specified size with an initial value specified by the minimum age and length of the new AgeBandMatrix created
   * \param sz is the size of the vector to be created
   * \param minage is the minimum age of the AgeBandMatrix to be created
   * \param minl is the IntVector of minimum lengths of the AgeBandMatrix to be created
   * \param lsize is the IntVector of sizes of the AgeBandMatrix to be created
   */
  AgeBandMatrixPtrVector(int sz, int minage, const IntVector& minl, const IntVector& lsize);
  /**
   * \brief This is the AgeBandMatrixPtrVector constructor that creates a copy of an existing AgeBandMatrixPtrVector
   * \param initial is the AgeBandMatrixPtrVector to copy
   */
  AgeBandMatrixPtrVector(const AgeBandMatrixPtrVector& initial);
  /**
   * \brief This is the AgeBandMatrixPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~AgeBandMatrixPtrVector();
  /**
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param value is the value that will be entered for the new entries
   */
  void resize(int add, AgeBandMatrix* value);
  /**
   * \brief This will add new empty entries to the vector
   * \param add is the number of new entries to the vector
   * \note The new elements of the vector will be created, and set to zero
   */
  void resize(int add);
  /**
   * \brief This will add new entries to the vector and create new AgeBandMatrix initial values
   * \param add is the number of new entries to the vector
   * \param minage is the minimum age of the AgeBandMatrix to be created
   * \param minl is the IntVector of minimum lengths of the AgeBandMatrix to be created
   * \param lsize is the IntVector of sizes of the AgeBandMatrix to be created
   */
  void resize(int add, int minage, const IntVector& minl, const IntVector& lsize);
  /**
   * \brief This will add new entries to the vector and create new AgeBandMatrix initial values
   * \param add is the number of new entries to the vector
   * \param minage is the minimum age of the AgeBandMatrix to be created
   * \param minl is the minimum length of the AgeBandMatrix to be created
   * \param matr is the PopInfoMatrix that the initial values are to be copied from
   */
  void resize(int add, int minage, int minl, const PopInfoMatrix& matr);
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
  AgeBandMatrix& operator [] (int pos) { return *v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const AgeBandMatrix& operator [] (int pos) const { return *v[pos]; };
  /**
   * \brief This function will implement the migration of entries from one AgeBandMatrix to another within the vector of values
   * \param MI is the DoubleMatrix describing the migration
   * \param tmp is the PopInfoVector used to temporarily store the population during migration
   */
  void Migrate(const DoubleMatrix& MI, PopInfoVector& tmp);
protected:
  /**
   * \brief This is the vector of AgeBandMatrix values
   */
  AgeBandMatrix** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
