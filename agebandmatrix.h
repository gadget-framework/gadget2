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
  AgeBandMatrix(int Minage, const IntVector& minl, const IntVector& size);
  AgeBandMatrix(int Minage, const PopInfoMatrix& initial);
  AgeBandMatrix(int Minage, int minl, const PopInfoMatrix& initial);
  AgeBandMatrix(int age, const PopInfoIndexVector& initial);
  AgeBandMatrix(const AgeBandMatrix& initial);
  AgeBandMatrix() { minage = 0; nrow = 0; v = 0; };
  ~AgeBandMatrix();
  int Minage() const { return minage; };
  int Maxage() const { return minage + nrow - 1; };
  int Nrow() const { return nrow; };
  PopInfoIndexVector& operator [] (int age);
  const PopInfoIndexVector& operator [] (int age) const;
  int Minlength(int age) const { return v[age - minage]->Mincol(); };
  int Maxlength(int age) const { return v[age - minage]->Maxcol(); };
  void Colsum(PopInfoVector& Result) const;
  void Multiply(const DoubleVector& Ratio, const ConversionIndex& CI);
  void Subtract(const DoubleVector& Consumption, const ConversionIndex& CI, const PopInfoVector& Nrof);
  void Multiply(const DoubleVector& NaturalM);
  void setToZero();
  void FilterN(double minN);
  void IncrementAge();
  void CopyNumbers(const AgeBandMatrix& Alkeys);
  void Grow(const DoubleMatrix& Lgrowth, const DoubleMatrix& Wgrowth);
  void Grow(const DoubleMatrix& Lgrowth, const DoubleMatrix& Wgrowth, Maturity* const Mat,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
  void Grow(const DoubleMatrix& Lgrowth, const DoubleVector& Weight);
  void Grow(const DoubleMatrix& Lgrowth, const DoubleVector& Weight, Maturity* const Mat,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
protected:
  int minage;
  int nrow;
  PopInfoIndexVector** v;
};

//And then we have some functions that are defined in a .cc file.
extern void AgebandmAdd(AgeBandMatrix& Alkeys, const AgeBandMatrix& Addition,
  const ConversionIndex& CI, double ratio = 1, int minage = 0, int maxage = 100);

#ifdef GADGET_INLINE
#include "agebandmatrix.icc"
#endif

#endif
