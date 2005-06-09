#ifndef agebandmatrix_h
#define agebandmatrix_h

#include "areatime.h"
#include "conversionindex.h"
#include "bandmatrix.h"
#include "popinfomatrix.h"
#include "popinfoindexvector.h"

class Maturity;

/**
 * \class AgeBandMatrix
 * \brief This class implements a vector of PopInfoIndexVector values, indexed from minage not 0
 */
class AgeBandMatrix {
public:
  AgeBandMatrix(int age, const IntVector& minl, const IntVector& size);
  AgeBandMatrix(int age, const PopInfoMatrix& initial);
  AgeBandMatrix(int age, int minl, const PopInfoMatrix& initial);
  AgeBandMatrix(int age, const PopInfoIndexVector& initial);
  AgeBandMatrix(const AgeBandMatrix& initial);
  AgeBandMatrix() { minage = 0; nrow = 0; v = 0; };
  ~AgeBandMatrix();
  int minAge() const { return minage; };
  int maxAge() const { return minage + nrow - 1; };
  int Nrow() const { return nrow; };
  PopInfoIndexVector& operator [] (int age) {
    assert(minage <= age && age < (minage + nrow));
    return *(v[age - minage]);
  };
  const PopInfoIndexVector& operator [] (int age) const {
    assert(minage <= age && age < (minage + nrow));
    return *(v[age - minage]);
  };
  int minLength(int age) const { return v[age - minage]->minCol(); };
  int maxLength(int age) const { return v[age - minage]->maxCol(); };
  void sumColumns(PopInfoVector& Result) const;
  void Multiply(const DoubleVector& Ratio, const ConversionIndex& CI);
  void Subtract(const DoubleVector& Consumption, const ConversionIndex& CI, const PopInfoVector& Nrof);
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
  void Add(const AgeBandMatrix& Addition, const ConversionIndex& CI, double ratio = 1.0,
    int minaddage = 0, int maxaddage = 9999);
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
