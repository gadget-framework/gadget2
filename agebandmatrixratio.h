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
  AgeBandMatrixRatio(int Minage, const IntVector& minl, const IntVector& size);
  AgeBandMatrixRatio(int Minage, const PopRatioMatrix& initial);
  AgeBandMatrixRatio(int age, const PopRatioIndexVector& initial);
  AgeBandMatrixRatio(const AgeBandMatrixRatio& initial);
  AgeBandMatrixRatio() { minage = 0; nrow = 0; v = 0; };
  ~AgeBandMatrixRatio();
  int Minage() const { return minage; };
  int Maxage() const { return minage + nrow - 1; };
  int Nrow() const { return nrow; };
  int Minlength(int age) const { return v[age - minage]->Mincol(); };
  int Maxlength(int age) const { return v[age - minage]->Maxcol(); };
  void IncrementAge(const AgeBandMatrix& Total);
  void Grow(const DoubleMatrix& Lgrowth, const AgeBandMatrix& Total);
  void Grow(const DoubleMatrix& Lgrowth, const AgeBandMatrix& Total,
    Maturity* const Mat, const TimeClass* const TimeInfo, const AreaClass* const Area,
    const LengthGroupDivision* const GivenLDiv, int area);
  PopRatioIndexVector& operator [] (int age);
  const PopRatioIndexVector& operator [] (int age) const;
  void UpdateRatio(const AgeBandMatrix& Total);
  void UpdateNumbers(const AgeBandMatrix& Total);
  void UpdateAndTagLoss(const AgeBandMatrix& Total, const DoubleVector& tagloss);
  int NrOfTagExp() const;
  void SettoZero();
protected:
  int minage;
  int nrow;
  PopRatioIndexVector** v;
};

#ifdef GADGET_INLINE
#include "agebandmatrixratio.icc"
#endif

#endif
