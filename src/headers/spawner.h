#ifndef spawner_h
#define spawner_h

#include "areatime.h"
#include "formulavector.h"
#include "modelvariablevector.h"
#include "commentstream.h"
#include "agebandmatrix.h"
#include "intmatrix.h"
#include "livesonareas.h"
#include "selectfunc.h"
#include "stock.h"

/**
 * \class SpawnData
 * \brief This is the class used to calculate the affect of the spawning process on the stock, possibly including the creation of new recruits into the model
 *
 * This class calculates the proportion of the stock that should be removed from the model due to the spawning process, and the affect that the spawning process has on the remaining stock.  This class can optionally add new recruits into the model due to the spawning process, implementing a closed life cycle model.  For the 'parent' stock, the spawning process can remove a proportion of the stock due to a spawning mortality, and reduce the weight of those that survive.  For the 'child' stock, the spawning process will create a number of recruits based on a recruitment function, and then add these into the model in a similar way to the RenewalData class.
 */
class SpawnData : public HasName, protected LivesOnAreas {
public:
  /**
   * \brief This is the SpawnData constructor
   * \param infile is the CommentStream to read the spawning data from
   * \param maxage is the maximum age of the spawning stock
   * \param LgrpDiv is the LengthGroupDivision that the spawning stock will be calculated on
   * \param Areas is the IntVector of areas that the spawning stock will be calculated on
   * \param Area is the AreaClass for the current model
   * \param givenname is the name of the stock for this SpawnData class
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  SpawnData(CommentStream& infile, int maxage, const LengthGroupDivision* const LgrpDiv,
    const IntVector& Areas, const AreaClass* const Area, const char* givenname,
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
  /**
   * \brief This will add the calculated spawned stock into the new stock age-length cells
   * \param area is the area that the spawning process is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   */
  void addSpawnStock(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This will calculate the
   * \param Alkeys is the AgeBandMatrix of the spawning stock
   * \param area is the area that the transition is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   */
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
  /**
   * \brief This function will calculate the number of recruits that will be created by the spawning process on the current timestep, for a given age-length cell of the spawning stock
   * \param age is the age of the spawning stock
   * \param len is the length of the spawning stock
   * \param number is the number of the spawning stock in the age-length cell
   * \param weight is the weight of the spawning stock in the age-length cell
   * \return number of recruits
   */
  double calcSpawnNumber(int age, int len, double number, double weight);
  /**
   * \brief This function will calculate the total number of recruits that have been created by the spawning process on the current timestep
   * \param temp is the current temperature
   * \param inarea is the current area identifier
   * \return total number of recruits
   */
  double calcRecruitNumber(double temp, int inarea);
  /**
   * \brief This is the StockPtrVector of the stocks that will be spawned
   */
  StockPtrVector spawnStocks;
  /**
   * \brief This is the CharPtrVector of the names of the spawned stocks
   */
  CharPtrVector spawnStockNames;
  /**
   * \brief This is the FormulaVector of the ratio of the new stock to move into each spawned stock
   */
  FormulaVector spawnRatio;
  /**
   * \brief This is used to scale the ratios to ensure that they will always sum to 1
   */
  double ratioscale;
  /**
   * \brief This is the IntVector used as an index for the ratio vector
   */
  IntVector ratioindex;
  /**
   * \brief This is the first year of the simulation that the spawning will take place on
   * \note This is optional, and will default to the first year of the simulation if it is not specified
   */
  int spawnFirstYear;
  /**
   * \brief This is the last year of the simulation that the spawning will take place on
   * \note This is optional, and will default to the last year of the simulation if it is not specified
   */
  int spawnLastYear;
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
  /**
   * \brief This is the DoubleMatrix used to store the numbers of the recruits, if the spawning process is to include the creation of the recruits
   */
  DoubleMatrixPtrVector spawnNumbers;
  /**
   * \brief This is the ModelVariableVector used to store the parameters when calculating the recruitment from the spawning process, if the spawning process is to include the creation of the recruits
   */
  ModelVariableVector spawnParameters;
  /**
   * \brief This is the ModelVariableVector used to store the parameters when calculating the recruits, if the spawning process is to include the creation of the recruits
   */
  ModelVariableVector stockParameters;
  /**
   * \brief This is the flag used to denote whether the spawning process should include the creation of the recruits
   */
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
