#ifndef agebandmatrixratio_h
#define agebandmatrixratio_h

#include "popratio.h"
#include "conversion.h"
#include "bandmatrix.h"
#include "popratiomatrix.h"
#include "popratioindexvector.h"
#include "agebandm.h"
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
  int Minlength(int age) const { return v[age-minage]->Mincol(); };
  int Maxlength(int age) const { return v[age-minage]->Maxcol(); };
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

class Agebandmatrixratiovector {
public:
  Agebandmatrixratiovector() { size = 0; v = 0; };
  Agebandmatrixratiovector(int size);
  Agebandmatrixratiovector(int size1, int Minage, const intvector& minl, const intvector& size2);
  ~Agebandmatrixratiovector();
  void ChangeElement(int nr, const Agebandmatrixratio& value);
  void resize(int add, Agebandmatrixratio* matr);
  void resize(int add, int minage, const intvector& minl, const intvector& size);
  void resize(int add, Agebandmatrixvector* initial, const Agebandmatrixvector& Alkeys, const char* id);
  void resize();
  void addTag(Agebandmatrixvector* initial, const Agebandmatrixvector& Alkeys, const char* id);
  void addTag(const char* name);
  void deleteTag(const char* tagname);
  void deleteTag(int id);
  int Size() const { return size; };
  int numTags() const { return tagid.Size(); };
  int getId(const char* id);
  const char* getName(int id) const;
  Agebandmatrixratio& operator [] (int pos);
  const Agebandmatrixratio& operator [] (const char* name) const;
  const Agebandmatrixratio& operator [] (int pos) const;
  void Migrate(const doublematrix& Migrationmatrix, const Agebandmatrixvector& Total);
  const int NrOfTagExp() const { return tagid.Size(); };
  void print(char* filename);
  const charptrvector tagids() const;
  friend void AgebandmratioAdd(Agebandmatrixratiovector& Alkeys, int AlkeysArea,
    const Agebandmatrixratiovector& Addition, int AdditionArea, const ConversionIndex &CI,
    double ratio, int minage, int maxage);
protected:
  int size;
  Agebandmatrixratio** v;
  charptrvector tagid;
private:
  void addTagName(const char* name);
};

#ifdef GADGET_INLINE
#include "agebandmatrixratio.icc"
#endif

#endif
