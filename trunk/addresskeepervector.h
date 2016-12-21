#ifndef addresskeepervector_h
#define addresskeepervector_h

#include "addresskeeper.h"

/**
 * \class AddressKeeperVector
 * \brief This class implements a dynamic vector of AddressKeeper values
 */
class AddressKeeperVector {
public:
  /**
   * \brief This is the default AddressKeeperVector constructor
   */
  AddressKeeperVector() { size = 0; v = 0; };
  /**
   * \brief This is the AddressKeeperVector destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~AddressKeeperVector();
  /**
   * \brief This will add one new empty entry to the vector
   * \note The new element of the vector will be created, and set to zero
   */
  void resize();
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
  AddressKeeper& operator [] (int pos) { return v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const AddressKeeper& operator [] (int pos) const { return v[pos]; };
protected:
  /**
   * \brief This is the vector of AddressKeeper values
   */
  AddressKeeper* v;
  /**
   * \brief This is the size of the vector
   */
  int size;
};

#endif
