#ifndef agebandmatrixratioptrvector_h
#define agebandmatrixratioptrvector_h

#include "popratio.h"
#include "bandmatrix.h"
#include "popratiomatrix.h"
#include "popratioindexvector.h"
#include "areatime.h"
#include "agebandmatrix.h"
#include "agebandmatrixptrvector.h"
#include "agebandmatrixratio.h"
#include "charptrvector.h"
#include "gadget.h"

class Maturity;

class AgeBandMatrixRatioPtrVector {
public:
  AgeBandMatrixRatioPtrVector() { size = 0; v = 0; };
  AgeBandMatrixRatioPtrVector(int size);
  AgeBandMatrixRatioPtrVector(int size1, int Minage, const IntVector& minl, const IntVector& size2);
  ~AgeBandMatrixRatioPtrVector();
  void ChangeElement(int nr, const AgeBandMatrixRatio& value);
  void resize(int add, AgeBandMatrixRatio* matr);
  void resize(int add, int minage, const IntVector& minl, const IntVector& size);
  void resize(int add, AgeBandMatrixPtrVector* initial, const AgeBandMatrixPtrVector& Alkeys, const char* id);
  void resize();
  void addTag(AgeBandMatrixPtrVector* initial, const AgeBandMatrixPtrVector& Alkeys, const char* id);
  void addTag(const char* name);
  void deleteTag(const char* tagname);
  void deleteTag(int id);
  int Size() const { return size; };
  int numTags() const { return tagid.Size(); };
  int getId(const char* id);
  const char* getName(int id) const;
  AgeBandMatrixRatio& operator [] (int pos);
  const AgeBandMatrixRatio& operator [] (const char* name) const;
  const AgeBandMatrixRatio& operator [] (int pos) const;
  void Migrate(const DoubleMatrix& Migrationmatrix, const AgeBandMatrixPtrVector& Total);
  const int NrOfTagExp() const { return tagid.Size(); };
  void print(char* filename);
  const CharPtrVector tagids() const;
  friend void AgebandmratioAdd(AgeBandMatrixRatioPtrVector& Alkeys, int AlkeysArea,
    const AgeBandMatrixRatioPtrVector& Addition, int AdditionArea, const ConversionIndex &CI,
    double ratio, int minage, int maxage);
protected:
  int size;
  AgeBandMatrixRatio** v;
  CharPtrVector tagid;
private:
  void addTagName(const char* name);
};

#ifdef GADGET_INLINE
#include "agebandmatrixratioptrvector.icc"
#endif

#endif
