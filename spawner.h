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
  /**
   * \brief This is the StockPtrVector of the stocks that will be spawned
   */
  StockPtrVector spawnStocks;
  /**
   * \brief This is the CharPtrVector of the names of the spawned stocks
   */
  CharPtrVector spawnStockNames;
  /**
   * \brief This is the DoubleVector of the ratio of the new stock to move into each spawned stock
   */
  DoubleVector Ratio;
  /**
   * \brief This is the IntVector used to store information about the steps when the spawning takes place
   */
  IntVector spawnStep;
  /**
   * \brief This is the IntVector used to store information about the areas where the spawning takes place
   */
  IntVector spawnArea;
  /**
   * \brief This is the LengthGroupDivision of the stock
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the DoubleVector of the proportion of each length group of the stock that will spawn
   */
  DoubleVector spawnProportion;
  /**
   * \brief This is the DoubleVector of the proportion of each length group of the stock that will die due to the spawning process
   */
  DoubleVector spawnMortality;
  /**
   * \brief This is the DoubleVector of the weight loss of each length group of the stock due to the spawning process
   */
  DoubleVector spawnWeightLoss;
  /**
   * \brief This is the SelectFunc used to calculate of the proportion of each length group of the stock that will spawn
   */
  SelectFunc* fnProportion;
  /**
   * \brief This is the SelectFunc used to calculate of the proportion of each length group of the stock that will die due to the spawning process
   */
  SelectFunc* fnMortality;
  /**
   * \brief This is the SelectFunc used to calculate of the weight loss of each length group of the stock due to the spawning process
   */
  SelectFunc* fnWeightLoss;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to store the calculated spawned stocks
   */
  AgeBandMatrixPtrVector Storage;
  /**
   * \brief This is the ConversionIndexPtrVector used to convert to the stock lengths for the spawned stock
   */
  ConversionIndexPtrVector CI;
  /**
   * \brief This is the LengthGroupDivision of the spawned stock
   */
  LengthGroupDivision* spawnLgrpDiv;
  DoubleMatrix spawnNumbers;
  TimeVariableVector spawnParameters;
  TimeVariableVector stockParameters;
  int onlyParent;
  /**
   * \brief This is the age that the spawned stock will move into the new stocks
   */
  int spawnAge;
  /**
   * \brief This is the identifier of the function to be used to calculate the number of recruits
   */
  int functionnumber;
  /**
   * \brief This is the name of the function to be used to calculate the number of recruits
   */
  char* functionname;
};

#endif
