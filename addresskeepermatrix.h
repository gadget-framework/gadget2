#ifndef addresskeepermatrix_h
#define addresskeepermatrix_h

#include "intvector.h"
#include "addresskeepervector.h"

/**
 * \class AddressKeeperMatrix
 * \brief This class implements a dynamic vector of AddressKeeperVector values
 */
class AddressKeeperMatrix {
public:
  /**
   * \brief This is the default AddressKeeperMatrix constructor
   */
  AddressKeeperMatrix() { nrow = 0; v = 0; };
  /**
   * \brief This is the AddressKeeperMatrix constructor for a specified size
   * \param nr is the size of the vector to be created
   * \param nc is the length of each row to be created (ie. the size of the AddressKeeperVector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  AddressKeeperMatrix(int nr, int nc);
  /**
   * \brief This is the AddressKeeperMatrix constructor for a specified size
   * \param nr is the size of the vector to be created
   * \param nc is the length of each row to be created (ie. the size of the AddressKeeperVector to be created for each row)
   * \param initial is the initial value for all the entries of the vector
   */
  AddressKeeperMatrix(int nr, int nc, AddressKeeper initial);
  /**
   * \brief This is the AddressKeeperMatrix constructor for a specified size
   * \param nr is the size of the vector to be created
   * \param nc is the length of the rows to be created (ie. the size of the AddressKeeperVector to be created for each row)
   * \note The elements of the vector will all be created, and set to zero
   */
  AddressKeeperMatrix(int nr, const IntVector& nc);
  /**
   * \brief This is the AddressKeeperMatrix constructor for a specified size
   * \param nr is the size of the vector to be created
   * \param nc is the length of the rows to be created (ie. the size of the AddressKeeperVector to be created for each row)
   * \param initial is the initial value for all the entries of the vector
   */
  AddressKeeperMatrix(int nr, const IntVector& nc, AddressKeeper initial);
  /**
   * \brief This is the AddressKeeperMatrix constructor that creates a copy of an existing AddressKeeperMatrix
   * \param initial is the AddressKeeperMatrix to copy
   */
  AddressKeeperMatrix(const AddressKeeperMatrix& initial);
  /**
   * \brief This is the AddressKeeperMatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~AddressKeeperMatrix();
  /**
   * \brief This will return the number of columns in row i of the vector
   * \param i is the row of the vector to have the number of columns counted
   * \return the number of columns in row i of the vector
   * \note This is the number of entries in the AddressKeeperVector that is entry i of the AddressKeeperMatrix
   */
  int Ncol(int i = 0) const { return v[i]->Size(); };
  /**
   * \brief This will return the number of rows of the vector
   * \return the number of rows of the vector
   */
  int Nrow() const { return nrow; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  AddressKeeperVector& operator [] (int pos) { return *v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const AddressKeeperVector& operator [] (int pos) const { return *v[pos]; };
  /**
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param length is the number of entries to the AddressKeeperVector that is created
   * \param initial is the value that will be entered for the new entries
   */
  void AddRows(int add, int length, AddressKeeper initial);
  /**
   * \brief This will add new empty entries to the vector
   * \param add is the number of new entries to the vector
   * \param length is the number of entries to the AddressKeeperVector that is created
   * \note The new elements of the vector will be created, and set to zero
   */
  void AddRows(int add, int length);
  /**
   * \brief This will delete an entry from the vector
   * \param row is the element of the vector to be deleted
   * \note This will free the memory allocated to the deleted element of the vector
   */
  void DeleteRow(int row);
protected:
  /**
   * \brief This is number of rows of the vector
   */
  int nrow;
  /**
   * \brief This is the vector of AddressKeeperVector values
   */
  AddressKeeperVector** v;
};

#endif
