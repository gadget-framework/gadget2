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
  AgeBandMatrixRatioPtrVector(int size1, int MinAge, const IntVector& minl, const IntVector& size2);
  ~AgeBandMatrixRatioPtrVector();
  void ChangeElement(int nr, const AgeBandMatrixRatio& value);
  void resize(int add, AgeBandMatrixRatio* matr);
  void resize(int add, int minage, const IntVector& minl, const IntVector& size);
  void addTag(AgeBandMatrixPtrVector* initial, const AgeBandMatrixPtrVector& Alkeys, const char* id, double tagloss);
  void addTag(const char* name);
  void deleteTag(const char* tagname);
  void deleteTag(int id);
  int Size() const { return size; };
  int getID(const char* id);
  const char* getName(int id) const;
  AgeBandMatrixRatio& operator [] (int pos) { return *v[pos]; };
  const AgeBandMatrixRatio& operator [] (int pos) const { return *v[pos]; };
  void Migrate(const DoubleMatrix& MI, const AgeBandMatrixPtrVector& Total);
  const int numTagExperiments() const { return tagID.Size(); };
  const CharPtrVector tagIDs() const { return tagID; };
  const DoubleVector& tagloss() const { return tagLoss; };
  friend void AgebandmratioAdd(AgeBandMatrixRatioPtrVector& Alkeys, int AlkeysArea,
    const AgeBandMatrixRatioPtrVector& Addition, int AdditionArea, const ConversionIndex &CI,
    double ratio, int minage, int maxage);
protected:
  int size;
  AgeBandMatrixRatio** v;
  CharPtrVector tagID;
  DoubleVector tagLoss;
private:
  void addTagName(const char* name);
};

#endif
