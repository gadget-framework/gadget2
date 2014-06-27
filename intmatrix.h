#ifndef intmatrix_h
#define intmatrix_h

#include "intvector.h"

/**
 * \class IntMatrix
 * \brief This class implements a dynamic vector of IntVector values
 */
class IntMatrix {
public:
  /**
   * \brief This is the default IntMatrix constructor
   */
  IntMatrix() { nrow = 0; v = 0; };
  /**
   * \brief This is the IntMatrix constructor for a specified size
   * \param nr is the size of the vector to be created
   * \param nc is the length of each row to be created (ie. the size of the IntVector to be created for each row)
   * \param initial is the initial value for all the entries of the vector
   */
  IntMatrix(int nr, int nc, int initial);
  /**
   * \brief This is the IntMatrix constructor that creates a copy of an existing IntMatrix
   * \param initial is the IntMatrix to copy
   */
  IntMatrix(const IntMatrix& initial);
  /**
   * \brief This is the IntMatrix destructor
   * \note This will free all the memory allocated to all the elements of the vector
   */
  ~IntMatrix();
  /**
   * \brief This will return the number of columns in row i of the vector
   * \param i is the row of the vector to have the number of columns counted
   * \return the number of columns in row i of the vector
   * \note This is the number of entries in the IntVector that is entry i of the IntMatrix
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
  IntVector& operator [] (int pos) { return *v[pos]; };
  /**
   * \brief This will return the value of an element of the vector
   * \param pos is the element of the vector to be returned
   * \return the value of the specified element
   */
  const IntVector& operator [] (int pos) const { return *v[pos]; };
  /**
   * \brief This will add new entries to the vector
   * \param add is the number of new entries to the vector
   * \param length is the number of entries to the IntVector that is created
   * \param value is the value that will be entered for the new entries
   */
  void AddRows(int add, int length, int value);
  /**
   * \brief This will delete an entry from the vector
   * \param pos is the element of the vector to be deleted
   * \note This will free the memory allocated to the deleted element of the vector
   */
  void Delete(int pos);
  /**
   * \brief This will reset the vector
   * \note This will delete every entry from the vector and set the number of rows to zero
   */
  void Reset();
  /**
   * \brief This function will set all of the entries of the vector to zero
   */
  void setToZero();
  /**
   * \brief This function will print the data stored in the vector
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile) const;
  /**
   * \brief This operator will set the vector equal to an existing IntMatrix
   * \param initial is the IntMatrix to copy
   */
  IntMatrix& operator = (const IntMatrix& initial);
protected:
  /**
   * \brief This is number of rows of the vector
   */
  int nrow;
  /**
   * \brief This is the vector of IntVector values
   */
  IntVector** v;
};

#endif
