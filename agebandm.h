#ifndef agebandm_h
#define agebandm_h

#include "conversion.h"
#include "bandmatrix.h"
#include "popinfomatrix.h"
#include "popinfoindexvector.h"

class AreaClass;
class TimeClass;
class Maturity;

class Agebandmatrix {
public:
  Agebandmatrix(int Minage, const intvector& minl, const intvector& size);
  Agebandmatrix(int Minage, const popinfomatrix& initial);
  Agebandmatrix(int Minage, int minl, const popinfomatrix& initial);
  Agebandmatrix(int age, const popinfoindexvector& initial);
  Agebandmatrix(const Agebandmatrix& initial);
  Agebandmatrix() { minage = 0; nrow = 0; v = 0; };
  ~Agebandmatrix();
  int Minage() const { return minage; };
  int Maxage() const { return minage + nrow - 1; };
  int Nrow() const { return nrow; };
  popinfoindexvector& operator [] (int age);
  const popinfoindexvector& operator [] (int age) const;
  int Minlength(int age) const { return v[age - minage]->Mincol(); };
  int Maxlength(int age) const { return v[age - minage]->Maxcol(); };
  void Colsum(popinfovector& Result) const;
  void Multiply(const doublevector& Ratio, const ConversionIndex& CI);
  void Subtract(const doublevector& Consumption, const ConversionIndex& CI, const popinfovector& Nrof);
  void Multiply(const doublevector& NaturalM);
  void SettoZero();
  void FilterN(double minN);
  void IncrementAge();
  void Grow(const doublematrix& Lgrowth, const doublematrix& Wgrowth);
  void Grow(const doublematrix& Lgrowth, const doublematrix& Wgrowth,
    Maturity* const Mat, const TimeClass* const TimeInfo, const AreaClass* const Area,
    const LengthGroupDivision* const GivenLDiv, int area);
  void Grow(const doublematrix& Lgrowth, const doublevector& Weight);
  void Grow(const doublematrix& Lgrowth, const doublevector& Weight,
    Maturity* const Mat, const TimeClass* const TimeInfo, const AreaClass* const Area,
    const LengthGroupDivision* const GivenLDiv, int area);
  friend ostream& printNorW(Agebandmatrix& a, ostream& stream, int PrintN, intvector& ages);
protected:
  int minage;
  int nrow;
  popinfoindexvector** v;
};

class Agebandmatrixvector {
public:
  Agebandmatrixvector() { size = 0; v = 0; };
  Agebandmatrixvector(int size);
  Agebandmatrixvector(int size1, int Minage, const intvector& minl, const intvector& size2);
  ~Agebandmatrixvector();
  void ChangeElement(int nr, const Agebandmatrix& value);
  void resize(int add, Agebandmatrix* matr);
  void resize(int add, int minage, int minl, const popinfomatrix& matr);
  void resize(int add, int minage, const intvector& minl, const intvector& size);
  int Size() const { return size; };
  Agebandmatrix& operator [] (int pos);
  const Agebandmatrix& operator [] (int pos) const;
  void Migrate(const doublematrix& Migrationmatrix);
protected:
  int size;
  Agebandmatrix** v;
};

//And then we have some functions that are defined in a .cc file.
extern void AgebandmSubtract(Agebandmatrix& Alkeys, const bandmatrix& Catch,
  const ConversionIndex& CI, int allowNegativeResults = 1);
extern void AgebandmAdd(Agebandmatrix& Alkeys, const Agebandmatrix& Addition,
  const ConversionIndex& CI, double ratio = 1, int minage = 0, int maxage = 100);

#ifdef GADGET_INLINE
#include "agebandm.icc"
#endif

#endif
