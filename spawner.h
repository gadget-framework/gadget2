#ifndef spawner_h
#define spawner_h

#include "areatime.h"
#include "formulamatrixptrvector.h"
#include "commentstream.h"
#include "agebandmatrix.h"
#include "intmatrix.h"
#include "livesonareas.h"
#include "actionattimes.h"

class Stock;
class Keeper;

/**
 * \class Spawner
 * \brief This is the class used to calculate the effect of the spawning on a stock
 */
class Spawner : protected LivesOnAreas {
public:
  /**
   * \brief This is the Spawner constructor
   * \param infile is the CommentStream to read the spawning data from
   * \param maxstockage is the maximum age of the stock
   * \param numlength is the number of length groups of the stock
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  Spawner(CommentStream& infile, int maxstockage, int numlength,
    const AreaClass* const Area, const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default Spawner destructor
   */
  ~Spawner();
  /**
   * \brief This function will print the spawning information
   * \param outfile is the ofstream that all the spawning information gets sent to
   */
  void Print(ofstream& outfile) const {};
  /**
   * \brief This is the function used to calculate the affects of spawning on a stock
   * \param Alkeys is the AgeBandMatrix for the stock which will be spawning
   * \param area is the internal area identifier for the spawning is taking place on
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  void Spawn(AgeBandMatrix& Alkeys, int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
protected:
  /**
   * \brief This is the function used to read in the spawning data from file
   * \param infile is the CommentStream to read the spawning data from
   * \param TimeInfo is the TimeClass for the current model
   * \param numarea is the number of areas
   * \param numage is the number of ages
   */
  void ReadSpawnerData(CommentStream& infile, const TimeClass* const TimeInfo, int numarea, int numage);
  /**
   * \brief This is the FormulaMatrixPtrVector of the ratio of the stock that will spawn
   */
  FormulaMatrixPtrVector spawnRatio;
  /**
   * \brief This is the FormulaMatrixPtrVector of the mortality to the stock due to spawning
   */
  FormulaMatrixPtrVector spawnMortality;
  /**
   * \brief This is the FormulaMatrixPtrVector of the weight loss from the stock due to spawning
   */
  FormulaMatrixPtrVector spawnWeightLoss;
  /**
   * \brief This is the matrix of areas that the spawning will take place on
   */
  IntMatrix spawnareas;
  /**
   * \brief This is the matrix of ages of the stock that will spawn
   */
  IntMatrix spawnages;
  /**
   * \brief This is the vector of names for the areas
   */
  CharPtrVector areaindex;
  /**
   * \brief This is the vector of names for the ages
   */
  CharPtrVector ageindex;
  /**
   * \brief This is the ActionAtTimes that the spawning will take place on
   */
  ActionAtTimes AAT;
  /**
   * \brief This is the vector of years that the spawning will take place on
   */
  IntVector Years;
  /**
   * \brief This is the vector of steps that the spawning will take place on
   */
  IntVector Steps;
  DoubleMatrix ssb;
};

#endif
