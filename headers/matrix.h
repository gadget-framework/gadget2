#ifndef matrix_h
#define matrix_h

#include "intvector.h"
#include "doublematrix.h"
/**
 * \class Matrix
 * \brief This class implements a Matrix of doubles, composed of an array of nc*nr doubles
 */
class Matrix {
public:
	/**
	 * \brief This is the default Matrix constructor
	 */
	Matrix() {
		nrow = 0;
		ncol = 0;
		m = 0;
	}
	;
	/**
	 * \brief This is the Matrix constructor for a specified size
	 * \param nr is the length of each row of the matrix
	 * \param nc is the length of each column of the matrix
	 * \param initial is the initial value for all the entries of the matrix
	 */
	Matrix(int nr, int nc, double initial);
	/**
	 * \brief This is the Matrix destructor
	 * \note This will free all the memory allocated to all the elements of the matrix
	 */
	~Matrix();
	/**
	 * \brief This will return the number of rows of the matrix
	 * \return the number of rows of the matrix
	 */
	int Nrow() const {
		return nrow;
	}
	;
	/**
	 * \brief This will return the number of columns of the matrix
	 * \return the number of columns of the matrix
	 */
	int Ncol() const {
		return ncol;
	}
	;
	/**
	 * \brief This will return the pointer to the row i of the matrix
	 * \param i is the row to be returned
	 * \return the pointer to the part of the array where the specified row begins
	 */
	double* operator [] (int i) { return m + (i*ncol); };
	/**
  	 * \brief This will return the pointer to the row i of the matrix
  	 * \param i is the row to be returned
  	 * \return the pointer to the part of the array where the specified row begins
  	 */
  const double* operator [] (int i) const { return m + (i*ncol); };
	/**
	 * \brief This will add new entries to the matrix
	 * \param add is the number of new entries to the matrix
	 * \param length is the number of entries to the Matrix that is created
	 * \param value is the value that will be entered for the new entries
	 */
	void AddRows(int add, int length, double value);
	/**
	 * \brief This will reset the matrix
	 * \note This will delete every entry from the matrix and set the number of rows and columns to zero
	 */
	void Reset();
	/**
	 * \brief This is will initialize the matrix to a specified size with a initial value
	 * \param nr is the length of each row of the matrix
	 * \param nc is the length of each column of the matrix
	 * \param initial is the initial value for all the entries of the matrix
	 */
	void Initialize(int nr, int nc, double initial);
	/**
	 * \brief This function will set all of the entries of the matrix to zero
	 */
	void setToZero();
	/**
	 * \brief This function will print the data stored in the matrix
	 * \param outfile is the ofstream that all the model information gets sent to
	 */
	void Print(ofstream& outfile) const;
protected:
	/**
	 * \brief This is number of rows of the matrix
	 */
	int nrow;
	/**
	 * \brief This is number of columns of the matrix
	 */
	int ncol;
	/**
	 * \brief This is the array of nr*nc doubles that represents the matrix
	 */
	double *m;
};

#endif
