#ifndef spawner_h
#define spawner_h

#include "formulamatrixptrvector.h"
#include "commentstream.h"
#include "agebandm.h"
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
  void Spawn(Agebandmatrix& Alkeys, int area, const AreaClass* const Area,
    const TimeClass* const TimeInfo);
  void Reset() {};
protected:
  void ReadSpawnerData(CommentStream&, const TimeClass*, int, int);
  Formulamatrixptrvector spawnRatio;      //[time][area][age]
  Formulamatrixptrvector spawnMortality;  //[time][area][age]
  Formulamatrixptrvector spawnWeightLoss; //[time][area][age]
  intmatrix areas;
  intmatrix ages;
  charptrvector areaindex;
  charptrvector ageindex;
  ActionAtTimes AAT;
  intvector Years;
  intvector Steps;
  doublematrix ssb;
};

#endif
