#ifndef maturity_h
#define maturity_h

#include "keeper.h"
#include "agebandmatrix.h"
#include "livesonareas.h"
#include "conversionindexptrvector.h"
#include "charptrvector.h"
#include "commentstream.h"
#include "agebandmatrixratioptrvector.h"
#include "timevariablevector.h"

class Maturity;

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

class MaturityA : public Maturity {
public:
  MaturityA(CommentStream& infile, const TimeClass* const TimeInfo,
    Keeper* const keeper, int minage, const IntVector& minabslength,
    const IntVector& size, const IntVector& areas,
    const LengthGroupDivision* const LgrpDiv, int NoMatconst);
  virtual ~MaturityA();
  virtual void Precalc(const TimeClass* const TimeInfo);
  virtual int IsMaturationStep(int area, const TimeClass* const TimeInfo);
  virtual double MaturationProbability(int age, int length, int Growth,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
  virtual void Print(ofstream& outfile) const;
protected:
  BandMatrix PrecalcMaturation;
  TimeVariableVector Coefficient;
  int MinMatureAge;
};

class MaturityB : public Maturity {
public:
  MaturityB(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper,
    int minage, const IntVector& minabslength, const IntVector& size,
    const IntVector& areas, const LengthGroupDivision* const LgrpDiv);
  virtual ~MaturityB();
  virtual void Precalc(const TimeClass* const TimeInfo);
  virtual int IsMaturationStep(int area, const TimeClass* const TimeInfo);
  virtual double MaturationProbability(int age, int length, int Growth,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
  virtual void Print(ofstream& outfile) const;
protected:
  IntVector maturitystep;
  TimeVariableVector maturitylength;
};

class MaturityC : public MaturityA {
public:
  MaturityC(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper,
    int minage, const IntVector& minabslength, const IntVector& size,
    const IntVector& areas, const LengthGroupDivision* const LgrpDiv, int NoMatconst);
  virtual ~MaturityC();
  virtual void Precalc(const TimeClass* const TimeInfo);
  virtual int IsMaturationStep(int area, const TimeClass* const TimeInfo);
  virtual double MaturationProbability(int age, int length, int Growth,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
protected:
  IntVector maturitystep;
};

class MaturityD : public MaturityA {
public:
  MaturityD(CommentStream& infile, const TimeClass* const TimeInfo, Keeper* const keeper,
    int minage, const IntVector& minabslength, const IntVector& size,
    const IntVector& areas, const LengthGroupDivision* const LgrpDiv, int NoMatconst);
  virtual ~MaturityD();
  virtual void Precalc(const TimeClass* const TimeInfo);
  virtual int IsMaturationStep(int area, const TimeClass* const TimeInfo);
  virtual double MaturationProbability(int age, int length, int Growth,
    const TimeClass* const TimeInfo, const AreaClass* const Area, int area);
protected:
  IntVector maturitystep;
};

#endif
