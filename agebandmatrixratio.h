#ifndef agebandmatrixratio_h
#define agebandmatrixratio_h

#include "popratio.h"
#include "conversion.h"
#include "bandmatrix.h"
#include "popratiomatrix.h"
#include "popratioindexvector.h"
#include "agebandmatrix.h"
#include "agebandmatrixptrvector.h"
#include "charptrvector.h"
#include "gadget.h"

class AreaClass;
class TimeClass;
class Maturity;

class Agebandmatrixratio {
public:
  Agebandmatrixratio(int Minage, const intvector& minl, const intvector& size);
  Agebandmatrixratio(int Minage, const popratiomatrix& initial);
  Agebandmatrixratio(int age, const popratioindexvector& initial);
  Agebandmatrixratio(const Agebandmatrixratio& initial);
  Agebandmatrixratio() { minage = 0; nrow = 0; v = 0; };
  ~Agebandmatrixratio();
  int Minage() const { return minage; };
  int Maxage() const { return minage + nrow - 1; };
  int Nrow() const { return nrow; };
  int Minlength(int age) const { return v[age - minage]->Mincol(); };
  int Maxlength(int age) const { return v[age - minage]->Maxcol(); };
  void IncrementAge(const Agebandmatrix& Total);
  void Grow(const doublematrix& Lgrowth, const Agebandmatrix& Total);
  void Grow(const doublematrix& Lgrowth, const Agebandmatrix& Total,
    Maturity* const Mat, const TimeClass* const TimeInfo, const AreaClass* const Area,
    const LengthGroupDivision* const GivenLDiv, int area);
  popratioindexvector& operator [] (int age);
  const popratioindexvector& operator [] (int age) const;
  void UpdateRatio(const Agebandmatrix& Total);
  void UpdateNumbers(const Agebandmatrix& Total);
  int NrOfTagExp() const;
protected:
  int minage;
  int nrow;
  popratioindexvector** v;
};

#ifdef GADGET_INLINE
#include "agebandmatrixratio.icc"
#endif

#endif
