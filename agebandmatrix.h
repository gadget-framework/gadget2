#ifndef agebandmatrix_h
#define agebandmatrix_h

#include "areatime.h"
#include "conversionindex.h"
#include "bandmatrix.h"
#include "popinfomatrix.h"
#include "popinfoindexvector.h"

class Maturity;

class AgeBandMatrix {
public:
  AgeBandMatrix(int MinAge, const IntVector& minl, const IntVector& size);
  AgeBandMatrix(int MinAge, const PopInfoMatrix& initial);
  AgeBandMatrix(int MinAge, int minl, const PopInfoMatrix& initial);
  AgeBandMatrix(int age, const PopInfoIndexVector& initial);
  AgeBandMatrix(const AgeBandMatrix& initial);
  AgeBandMatrix() { minage = 0; nrow = 0; v = 0; };
  ~AgeBandMatrix();
  int minAge() const { return minage; };
  int maxAge() const { return minage + nrow - 1; };
  int Nrow() const { return nrow; };
  PopInfoIndexVector& operator [] (int age);
  const PopInfoIndexVector& operator [] (int age) const;
  int minLength(int age) const { return v[age - minage]->minCol(); };
  int maxLength(int age) const { return v[age - minage]->maxCol(); };
  void sumColumns(PopInfoVector& Result) const;
  void Multiply(const DoubleVector& Ratio, const ConversionIndex& CI);
  void Subtract(const DoubleVector& Consumption, const ConversionIndex& CI, const PopInfoVector& Nrof);
  void Multiply(const DoubleVector& Ratio);
  void setToZero();
  void FilterN(double minN);
  void IncrementAge();
  void printNumbers(ofstream& outfile) const;
  void printWeights(ofstream& outfile) const;
  void Grow(const DoubleMatrix& Lgrowth, const DoubleMatrix& Wgrowth);
  void Grow(const DoubleMatrix& Lgrowth, const DoubleMatrix& Wgrowth, Maturity* const Mat,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
  void Grow(const DoubleMatrix& Lgrowth, const DoubleVector& Weight);
  void Grow(const DoubleMatrix& Lgrowth, const DoubleVector& Weight, Maturity* const Mat,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
  void Add(const AgeBandMatrix& Addition, const ConversionIndex& CI, double ratio = 1.0,
    int minaddage = 0, int maxaddage = 9999);
protected:
  int minage;
  int nrow;
  PopInfoIndexVector** v;
};

#ifdef GADGET_INLINE
#include "agebandmatrix.icc"
#endif

#endif
