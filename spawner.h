#ifndef spawner_h
#define spawner_h

#include "formulamatrixptrvector.h"
#include "commentstream.h"
#include "agebandmatrix.h"
#include "intmatrix.h"
#include "livesonareas.h"
#include "actionattimes.h"

class AreaClass;
class TimeClass;
class Stock;
class Keeper;

class Spawner : protected LivesOnAreas {
public:
  Spawner(CommentStream& infile, int maxstockage, int numlength,
    const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper);
  ~Spawner();
  void Print(ofstream& outfile) const {};
  void Spawn(AgeBandMatrix& Alkeys, int area, const AreaClass* const Area,
    const TimeClass* const TimeInfo);
  void Reset() {};
protected:
  void ReadSpawnerData(CommentStream&, const TimeClass*, int, int);
  FormulaMatrixPtrVector spawnRatio;      //[time][area][age]
  FormulaMatrixPtrVector spawnMortality;  //[time][area][age]
  FormulaMatrixPtrVector spawnWeightLoss; //[time][area][age]
  IntMatrix areas;
  IntMatrix ages;
  CharPtrVector areaindex;
  CharPtrVector ageindex;
  ActionAtTimes AAT;
  IntVector Years;
  IntVector Steps;
  DoubleMatrix ssb;
};

#endif
