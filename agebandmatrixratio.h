#ifndef agebandmatrixratio_h
#define agebandmatrixratio_h

#include "popratio.h"
#include "lengthgroup.h"
#include "bandmatrix.h"
#include "popratiomatrix.h"
#include "popratioindexvector.h"
#include "agebandmatrix.h"
#include "agebandmatrixptrvector.h"
#include "charptrvector.h"
#include "gadget.h"

class Maturity;

class AgeBandMatrixRatio {
public:
  AgeBandMatrixRatio(int MinAge, const IntVector& minl, const IntVector& size);
  AgeBandMatrixRatio(int MinAge, const PopRatioMatrix& initial);
  AgeBandMatrixRatio(int MinAge, const PopRatioIndexVector& initial);
  AgeBandMatrixRatio(const AgeBandMatrixRatio& initial);
  AgeBandMatrixRatio() { minage = 0; nrow = 0; v = 0; };
  ~AgeBandMatrixRatio();
  int minAge() const { return minage; };
  int maxAge() const { return minage + nrow - 1; };
  int Nrow() const { return nrow; };
  int minLength(int age) const { return v[age - minage]->Mincol(); };
  int maxLength(int age) const { return v[age - minage]->Maxcol(); };
  void IncrementAge(const AgeBandMatrix& Total);
  void Grow(const DoubleMatrix& Lgrowth, const AgeBandMatrix& Total);
  void Grow(const DoubleMatrix& Lgrowth, const AgeBandMatrix& Total, Maturity* const Mat,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
  PopRatioIndexVector& operator [] (int age);
  const PopRatioIndexVector& operator [] (int age) const;
  void updateRatio(const AgeBandMatrix& Total);
  void updateNumbers(const AgeBandMatrix& Total);
  void updateAndTagLoss(const AgeBandMatrix& Total, const DoubleVector& tagloss);
  int numTagExperiments() const;
  void setToZero();
protected:
  int minage;
  int nrow;
  PopRatioIndexVector** v;
};

#ifdef GADGET_INLINE
#include "agebandmatrixratio.icc"
#endif

#endif
