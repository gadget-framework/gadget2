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

class SpawnData : protected LivesOnAreas {
public:
  SpawnData(CommentStream& infile, int maxage, const LengthGroupDivision* const LgrpDiv,
    const IntVector& Areas, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default SpawnData destructor
   */
  ~SpawnData();
  /**
   * \brief This will select the stocks required for the spawning process
   * \param stockvec is the StockPtrVector of all the available stocks
   */
  void setStock(StockPtrVector& stockvec);
  void addSpawnStock(int area, const TimeClass* const TimeInfo);
  void Spawn(AgeBandMatrix& Alkeys, int area, const TimeClass* const TimeInfo);
  /**
   * \brief This will check if the spawning process will take place on the current timestep and area
   * \param area is the area that the spawning is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the spawning process will take place, 0 otherwise
   */
  int isSpawnStepArea(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will print the spawning information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile) const;
  /**
   * \brief This function will reset the spawning data
   */
  void Reset(const TimeClass* const TimeInfo);
protected:
  double calcSpawnNumber(int age, int len, double number, double weight);
  double calcRecruitNumber();
  StockPtrVector spawnStocks;
  CharPtrVector spawnStockNames;
  DoubleVector Ratio;
  IntVector spawnStep;
  IntVector spawnArea;
  LengthGroupDivision* LgrpDiv;
  DoubleVector spawnProportion;
  DoubleVector spawnMortality;
  DoubleVector spawnWeightLoss;
  SelectFunc* fnProportion;
  SelectFunc* fnMortality;
  SelectFunc* fnWeightLoss;
  AgeBandMatrixPtrVector Storage;
  ConversionIndexPtrVector CI;
  DoubleMatrix spawnNumbers;
  LengthGroupDivision* spawnLgrpDiv;
  TimeVariableVector spawnParameters;
  TimeVariableVector stockParameters;
  int onlyParent;
  int spawnAge;
};

#endif
