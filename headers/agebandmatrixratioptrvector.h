#ifndef agebandmatrixratioptrvector_h
#define agebandmatrixratioptrvector_h

#include "popratio.h"
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
  void resize(int add, AgeBandMatrixRatio* matr);
  void resize(int add, int minage, const IntVector& minl, const IntVector& size);
  void addTag(AgeBandMatrixPtrVector* initial, const AgeBandMatrixPtrVector& Alkeys, const char* tagname, double tagloss);
  void addTag(const char* tagname);
  void deleteTag(const char* tagname);
  int Size() const { return size; };
  int getTagID(const char* tagname);
  const char* getTagName(int id) const { return tagID[id]; };
  void Migrate(const DoubleMatrix& MI, const AgeBandMatrixPtrVector& Total);
  int numTagExperiments() const { return tagID.Size(); };
  const DoubleVector& getTagLoss() const { return tagLoss; };
  AgeBandMatrixRatio& operator [] (int pos) { return *v[pos]; };
  const AgeBandMatrixRatio& operator [] (int pos) const { return *v[pos]; };
  void Add(const AgeBandMatrixRatioPtrVector& Addition, int area,
    const ConversionIndex &CI, double ratio = 1.0);
protected:
  void addTagName(const char* name);
  int size;
  AgeBandMatrixRatio** v;
  CharPtrVector tagID;
  DoubleVector tagLoss;
};

#endif
