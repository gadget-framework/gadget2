#ifndef otherfoodptrvector_h
#define otherfoodptrvector_h

#include "otherfood.h"

class OtherFood;

/**
 * \class OtherFoodPtrVector
 * \brief This class implements a dynamic vector of OtherFood values
 */
class OtherFoodPtrVector {
public:
  /**
   * \brief This is the default OtherFoodPtrVector constructor
   */
  OtherFoodPtrVector() { size = 0; v = 0; };
  /**
   * \brief This is the OtherFoodPtrVector constructor that creates a copy of an existing OtherFoodPtrVector
   * \param initial is the OtherFoodPtrVector to copy
   */
  OtherFoodPtrVector(const OtherFoodPtrVector& initial);
  /**
   * \brief This is the OtherFoodPtrVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~OtherFoodPtrVector();
  /**
   * \brief This will add one new entry to the vector
   * \param value is the value that will be entered for the new entry
   */
  void resize(OtherFood* value);
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
  OtherFood*& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  OtherFood* const& operator [] (int pos) const { return v[pos]; };
protected:
  /**
   * \brief This is the vector of OtherFood values
   */
  OtherFood** v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
