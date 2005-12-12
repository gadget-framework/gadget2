#ifndef agebandmatrix_h
#define agebandmatrix_h

#include "areatime.h"
#include "conversionindex.h"
#include "popinfomatrix.h"
#include "popinfoindexvector.h"

class Maturity;

/**
 * \class AgeBandMatrix
 * \brief This class implements a vector of PopInfoIndexVector values, indexed from minage not 0
 */
class AgeBandMatrix {
public:
  /**
   * \brief This is the default AgeBandMatrix constructor
   */
  AgeBandMatrix() { minage = 0; nrow = 0; v = 0; };
  AgeBandMatrix(int age, const IntVector& minl, const IntVector& size);
  AgeBandMatrix(int age, const PopInfoMatrix& initial, int minl = 0);
  AgeBandMatrix(int age, const PopInfoIndexVector& initial);
  /**
   * \brief This is the AgeBandMatrix constructor that creates a copy of an existing AgeBandMatrix
   * \param initial is the AgeBandMatrix to copy
   */
  AgeBandMatrix(const AgeBandMatrix& initial);
  /**
   * \brief This is the AgeBandMatrix destructor
   */
  ~AgeBandMatrix();
  int minAge() const { return minage; };
  int maxAge() const { return minage + nrow - 1; };
  /**
   * \brief This will return the number of rows of the vector
   * \return the number of rows of the vector
   */
  int Nrow() const { return nrow; };
  /**
   * \brief This will return the value of an element of the vector
   * \param age is the element of the vector to be returned
   * \return the value of the specified element
   */
  PopInfoIndexVector& operator [] (int age) { return *(v[age - minage]); };
  /**
   * \brief This will return the value of an element of the vector
   * \param age is the element of the vector to be returned
   * \return the value of the specified element
   */
  const PopInfoIndexVector& operator [] (int age) const { return *(v[age - minage]); };
  int minLength(int age) const { return v[age - minage]->minCol(); };
  int maxLength(int age) const { return v[age - minage]->maxCol(); };
  void sumColumns(PopInfoVector& Result) const;
  void Subtract(const DoubleVector& Ratio, const ConversionIndex& CI);
  void Multiply(const DoubleVector& Ratio);
  void setToZero();
  void IncrementAge();
  void printNumbers(ofstream& outfile) const;
  void printWeights(ofstream& outfile) const;
  void Grow(const DoubleMatrix& Lgrowth, const DoubleMatrix& Wgrowth);
  void Grow(const DoubleMatrix& Lgrowth, const DoubleMatrix& Wgrowth,
    Maturity* const Mat, const TimeClass* const TimeInfo, int area);
  void Grow(const DoubleMatrix& Lgrowth, const DoubleVector& Weight);
  void Grow(const DoubleMatrix& Lgrowth, const DoubleVector& Weight,
    Maturity* const Mat, const TimeClass* const TimeInfo, int area);
  void Add(const AgeBandMatrix& Addition, const ConversionIndex& CI, double ratio = 1.0);
protected:
  /**
   * \brief This is the index for the vector
   */
  int minage;
  /**
   * \brief This is number of rows of the vector
   */
  int nrow;
  /**
   * \brief This is the indexed vector of PopInfoIndexVector values
   */
  PopInfoIndexVector** v;
};

#endif
