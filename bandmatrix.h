#ifndef bandmatrix_h
#define bandmatrix_h

#include "doublematrix.h"
#include "doubleindexvector.h"

/**
 * \class BandMatrix
 * \brief This class implements a vector of DoubleIndexVector values, indexed from pos not 0
 */
class BandMatrix {
public:
  /**
   * \brief This is the default BandMatrix constructor
   */
  BandMatrix() { pos = 0; nrow = 0; v = 0; };
  /**
   * \brief This is the BandMatrix constructor that creates a copy of an existing BandMatrix
   * \param initial is the BandMatrix to copy
   */
  BandMatrix(const BandMatrix& initial);
  /**
   * \brief This is the BandMatrix constructor for a specified size with an initial value
   * \param minl is the IntVector of the minpos of the DoubleIndexVector values created
   * \param size is the IntVector of the size of the DoubleIndexVector values created
   * \param minpos is the index for the vector to be created (default value 0)
   * \param initial is the initial value for all the entries of the vector (default value 0.0)
   */
  BandMatrix(const IntVector& minl, const IntVector& size, int minpos = 0, double initial = 0.0);
  /**
   * \brief This is the BandMatrix constructor for a specified size with an initial value
   * \param minl is the minpos of all the DoubleIndexVector values created
   * \param size is the size of all the size of the DoubleIndexVector values created
   * \param minpos is the index for the vector to be created
   * \param nr is the size of the vector to be created
   * \param initial is the initial value for all the entries of the vector (default value 0.0)
   */
  BandMatrix(int minl, int size, int minpos, int nr, double initial = 0.0);
  /**
   * \brief This is the BandMatrix constructor, based on an existing DoubleMatrix
   * \param initial is the DoubleMatrix to copy
   * \param minpos is the index for the vector to be created (default value 0)
   * \param minl is the minpos of all the DoubleIndexVector values created (default value 0)
   */
  BandMatrix(const DoubleMatrix& initial, int minpos = 0, int minl = 0);
  /**
   * \brief This is the BandMatrix destructor
   */
  ~BandMatrix();
  /**
   * \brief This will return the value of an element of the vector
   * \param i is the element of the vector to be returned
   * \return the value of the specified element
   */
  DoubleIndexVector& operator [] (int i) {
    assert(pos <= i && i < (pos + nrow));
    return *(v[i - pos]);
  };
  /**
   * \brief This will return the value of an element of the vector
   * \param i is the element of the vector to be returned
   * \return the value of the specified element
   */
  const DoubleIndexVector& operator [] (int i) const {
    assert(pos <= i && i < (pos + nrow));
    return *(v[i - pos]);
  };
  /**
   * \brief This will return the number of rows of the vector
   * \return the number of rows of the vector
   */
  int Nrow() const { return nrow; };
  /**
   * \brief This will return the number of columns in row i of the vector
   * \param i is the row of the vector to have the number of columns counted
   * \return the number of columns in row i of the vector
   * \note This is the number of entries in the DoubleIndexVector that is entry i of the BandMatrix
   */
  int Ncol(int i) const { return v[i - pos]->Size(); };
  /**
   * \brief This will return the minimum row of the vector that is not null
   * \return the minimum row of the vector
   */
  int minRow() const { return pos; };
  /**
   * \brief This will return the maximum row of the vector that is not null
   * \return the maximum row of the vector
   */
  int maxRow() const { return pos + nrow - 1; };
  /**
   * \brief This will return the minimum column in row i that is not null
   * \param i is the row of the vector to minimum column index
   * \return the minimum column of row i
   */
  int minCol(int i) const { return v[i - pos]->minCol(); };
  /**
   * \brief This will return the maximum column in row i that is not null
   * \param i is the row of the vector to maximum column index
   * \return the minimum column of row i
   */
  int maxCol(int i) const { return v[i - pos]->maxCol(); };
  /**
   * \brief This function will print the data stored in the vector
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile) const;
protected:
  /**
   * \brief This is number of rows of the vector
   */
  int nrow;
  /**
   * \brief This is the index for the vector
   */
  int pos;
  /**
   * \brief This is the indexed vector of DoubleIndexVector values
   */
  DoubleIndexVector** v;
};

#endif
