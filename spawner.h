#ifndef spawner_h
#define spawner_h

#include "areatime.h"
#include "timevariablevector.h"
#include "commentstream.h"
#include "agebandmatrix.h"
#include "intmatrix.h"
#include "livesonareas.h"
#include "selectfunc.h"
#include "stock.h"

class Spawner : protected LivesOnAreas {
public:
  Spawner(CommentStream& infile, int maxage, const LengthGroupDivision* const LgrpDiv,
    const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper);
  ~Spawner();
  void Print(ofstream& outfile) const;
  void setStock(StockPtrVector& stockvec);
  void addSpawnStock(int area, const TimeClass* const TimeInfo);
  void Spawn(AgeBandMatrix& Alkeys, int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  int IsSpawnStepArea(int area, const TimeClass* const TimeInfo);
  void Reset(const TimeClass* const TimeInfo);
  double ssbFunc(int age, int len, double number, double weight);
protected:
  StockPtrVector SpawnStocks;
  CharPtrVector SpawnStockNames;
  DoubleVector Ratio;
  IntVector spawnstep;
  IntVector spawnarea;
  LengthGroupDivision* LgrpDiv;
  DoubleVector spawnProportion;
  DoubleVector spawnMortality;
  DoubleVector spawnWeightLoss;
  SelectFunc* fnProportion;
  SelectFunc* fnMortality;
  SelectFunc* fnWeightLoss;
  AgeBandMatrixPtrVector Storage;
  ConversionIndexPtrVector CI;
  DoubleMatrix ssb;
  LengthGroupDivision* spawnLgrpDiv;
  TimeVariableVector spawnParameters;
  TimeVariableVector stockParameters;
  int onlyParent;
  int spawnage;
};

#endif
