#ifndef spawner_h
#define spawner_h

#include "livesonareas.h"
#include "agebandm.h"
#include "commentstream.h"
#include "formula.h"

class AreaClass;
class TimeClass;
class Stock;
class Keeper;

class Spawner : protected LivesOnAreas {
public:
  Spawner(CommentStream& infile, int minage, int maxage, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  void Spawn(Agebandmatrix& Alkeys, int area, const AreaClass* const Area,
    const TimeClass* const TimeInfo);
protected:
  Formula spawningMortality;
  Formula spawningWeightLoss;
  doubleindexvector Spawningstep;     //[age]
  doubleindexvector Spawningratio;    //[age]
  doubleindexvector SpawningmortalityPattern;  //[age]
  doubleindexvector SpawningweightlossPattern; //[age]
};

#endif
