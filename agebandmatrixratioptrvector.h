#ifndef agebandmatrixratioptrvector_h
#define agebandmatrixratioptrvector_h

#include "popratio.h"
#include "conversion.h"
#include "bandmatrix.h"
#include "popratiomatrix.h"
#include "popratioindexvector.h"
#include "agebandmatrix.h"
#include "agebandmatrixptrvector.h"
#include "agebandmatrixratio.h"
#include "charptrvector.h"
#include "gadget.h"

class AreaClass;
class TimeClass;
class Maturity;

class agebandmatrixratioptrvector {
public:
  agebandmatrixratioptrvector() { size = 0; v = 0; };
  agebandmatrixratioptrvector(int size);
  agebandmatrixratioptrvector(int size1, int Minage, const intvector& minl, const intvector& size2);
  ~agebandmatrixratioptrvector();
  void ChangeElement(int nr, const Agebandmatrixratio& value);
  void resize(int add, Agebandmatrixratio* matr);
  void resize(int add, int minage, const intvector& minl, const intvector& size);
  void resize(int add, agebandmatrixptrvector* initial, const agebandmatrixptrvector& Alkeys, const char* id);
  void resize();
  void addTag(agebandmatrixptrvector* initial, const agebandmatrixptrvector& Alkeys, const char* id);
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
  void Migrate(const doublematrix& Migrationmatrix, const agebandmatrixptrvector& Total);
  const int NrOfTagExp() const { return tagid.Size(); };
  void print(char* filename);
  const charptrvector tagids() const;
  friend void AgebandmratioAdd(agebandmatrixratioptrvector& Alkeys, int AlkeysArea,
    const agebandmatrixratioptrvector& Addition, int AdditionArea, const ConversionIndex &CI,
    double ratio, int minage, int maxage);
protected:
  int size;
  Agebandmatrixratio** v;
  charptrvector tagid;
private:
  void addTagName(const char* name);
};

#ifdef GADGET_INLINE
#include "agebandmatrixratioptrvector.icc"
#endif

#endif
