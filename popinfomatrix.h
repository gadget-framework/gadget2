#ifndef popinfomatrix_h
#define popinfomatrix_h

#include "intvector.h"
#include "popinfovector.h"

/**
 * \class PopInfoMatrix
 * \brief This class implements a dynamic vector of PopInfoVector values
 */
class PopInfoMatrix {
public:
  /**
   * \brief This is the default PopInfoMatrix constructor
   */
  PopInfoMatrix() { nrow = 0; v = 0; };
  /**
   * \brief This is the PopInfoMatrix constructor for a specified size
   * \param nr is the size of the vector to be created
   * \param nc is the length of each row to be created (ie. the size of the PopInfoVector to be created for each row)
   * \param initial is the initial value for all the entries of the vector
   */
  PopInfoMatrix(int nr, int nc, PopInfo initial);
  /**
   * \brief This is the PopInfoMatrix constructor that creates a copy of an existing PopInfoMatrix
   * \param initial is the PopInfoMatrix to copy
   */
  PopInfoMatrix(const PopInfoMatrix& initial);
  /**
   * \brief This is the PopInfoMatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~PopInfoMatrix();
  /**
   * \brief This will return the number of columns in row i of the vector
   * \param i is the row of the vector to have the number of columns counted
   * \return the number of columns in row i of the vector
   * \note This is the number of entries in the PopInfoVector that is entry i of the PopInfoMatrix
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
  PopInfoVector& operator [] (int pos) { return *v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const PopInfoVector& operator [] (int pos) const { return *v[pos]; };
  /**
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param length is the number of entries to the PopInfoVector that is created
   * \param value is the value that will be entered for the new entries
   */
  void AddRows(int add, int length, PopInfo value);
  /**
   * \brief This will delete an entry from the vector
   * \param pos is the element of the vector to be deleted
   * \note This will free the memory allocated to the deleted element of the vector
   */
  void Delete(int pos);
protected:
  /**
   * \brief This is number of rows of the vector
   */
  int nrow;
  /**
   * \brief This is the vector of PopInfoVector values
   */
  PopInfoVector** v;
};

#endif
