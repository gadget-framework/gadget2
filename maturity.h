#ifndef maturity_h
#define maturity_h

#include "keeper.h"
#include "agebandmatrix.h"
#include "livesonareas.h"
#include "conversionindexptrvector.h"
#include "charptrvector.h"
#include "commentstream.h"
#include "agebandmatrixratioptrvector.h"

class Maturity;

typedef double Maturityfunc(int, int, int, const TimeClass* const, const AreaClass* const,
  const LengthGroupDivision* const, int, const DoubleVector&, const DoubleVector&, int);
typedef Maturityfunc* MaturityfuncPtr;

class Maturity: protected LivesOnAreas {
public:
  Maturity();
  Maturity(const IntVector& areas, int minage, const IntVector& minabslength,
    const IntVector& size, const LengthGroupDivision* const LgrpDiv);
  virtual ~Maturity();
  void SetStock(StockPtrVector& stockvec);
  virtual void Print(ofstream& outfile) const;
  virtual void Precalc(const TimeClass* const TimeInfo);
  virtual int IsMaturationStep(int area, const TimeClass* const TimeInfo) = 0;
  virtual double MaturationProbability(int age, int length, int Growth,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area) = 0;
  void PutInStorage(int area, int age, int length, double number,
    double weight, const TimeClass* const TimeInfo);
  void Move(int area, const TimeClass* const TimeInfo);
  void PutInStorage(int area, int age, int length, double number,
    const TimeClass* const TimeInfo, int id);
  int NoOfMatureStocks() { return MatureStocks.Size(); };
  const StockPtrVector& GetMatureStocks();
  void DeleteTag(const char* tagname);
  void AddTag(const char* tagname);
protected:
  StockPtrVector MatureStocks;
  CharPtrVector MatureStockNames;
  DoubleVector Ratio;
  ConversionIndexPtrVector CI;
  LengthGroupDivision* LgrpDiv;
private:
  AgeBandMatrixPtrVector Storage;
  AgeBandMatrixRatioPtrVector TagStorage;
};

#endif
