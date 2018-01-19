#ifndef stock_h
#define stock_h

#include "areatime.h"
#include "agebandmatrix.h"
#include "agebandmatrixratioptrvector.h"
#include "base.h"
#include "conversionindex.h"
#include "stockptrvector.h"
#include "tagptrvector.h"
#include "commentstream.h"

class Stock;
class Keeper;
class NaturalMortality;
class Grower;
class InitialCond;
class Migration;
class Transition;
class PopPredator;
class StockPrey;
class Maturity;
class RenewalData;
class SpawnData;
class StrayData;

/**
 * \class Stock
 * \brief This is the class used to model a stock within the Gadget model
 *
 * This class is used to model the population of a stock during a model simulation.
 */
class Stock : public BaseClass {
public:
  /**
   * \brief This is the Stock constructor
   * \param infile is the CommentStream to read the stock parameters from
   * \param givenname is the name of the stock
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  Stock(CommentStream& infile, const char* givenname, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default Stock constructor
   * \param givenname is the name of the stock
   */
  Stock(const char* givenname);
  /**
   * \brief This is the default Stock destructor
   */
  virtual ~Stock();
  /**
   * \brief This function will update the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void calcNumbers(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will reduce the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void reducePop(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the growth of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Grow(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate any transition of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updateAgePart1(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the age increase of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updateAgePart2(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will implement the transiton of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updateAgePart3(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the spawning of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart1(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate add the newly matured stock into the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart2(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate add the new recruits into the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart3(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate calculate the straying of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart4(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate add the strayed stock into the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart5(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void calcEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will check the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void checkEat(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will adjust the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void adjustEat(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will reset the model population
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Reset(const TimeClass* const TimeInfo);
  /**
   * \brief This function will print the model population
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile) const;
  /**
   * \brief This function will implement the migration of the model population for the model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Migrate(const TimeClass* const TimeInfo);
  /**
   * \brief This function will add a AgeBandMatrix to the current stock population
   * \param Addition is the AgeBandMatrix that will be added to the population
   * \param CI is the ConversionIndex that will convert between the length groups
   * \param area is an integer to denote the internal area of interest
   * \param ratio is a multiplicative constant applied
   */
  void Add(const AgeBandMatrix& Addition, const ConversionIndex* const CI, int area, double ratio);
  /**
   * \brief This function will add a AgeBandMatrixRatioPtrVector to the current tagged stock population
   * \param Addition is the AgeBandMatrixRatioPtrVector that will be added to the tagged population
   * \param CI is the ConversionIndex that will convert between the length groups
   * \param area is an integer to denote the internal area of interest
   * \param ratio is a multiplicative constant applied
   */
  void Add(const AgeBandMatrixRatioPtrVector& Addition, const ConversionIndex* const CI, int area, double ratio);
  /**
   * \brief This will return the prey information for the stock
   * \return prey
   */
  StockPrey* getPrey() const;
  /**
   * \brief This will return the migration information for the stock
   * \return migration
   */
  Migration* getMigration() const;
  /**
   * \brief This will return the predation information for the stock
   * \return predator
   */
  PopPredator* getPredator() const;
  /**
   * \brief This will return the current population of the stock on a given area
   * \param area is the area identifier
   * \return alkeys, a AgeBandMatrix containing the population of the stock
   */
  AgeBandMatrix& getCurrentALK(int area) { return Alkeys[this->areaNum(area)]; };
  /**
   * \brief This will return the population of the stock on a given area at the start of the current timestep
   * \param area is the area identifier
   * \return alkeys, a AgeBandMatrix containing the population of the stock
   */
  AgeBandMatrix& getConsumptionALK(int area);
  /**
   * \brief This will return the length group information for the stock
   * \return LgrpDiv
   */
  const LengthGroupDivision* getLengthGroupDiv() const { return LgrpDiv; };
  /**
   * \brief This function will initialise the stock and set-up links to any related stocks
   * \param stockvec is the StockPtrVector of all the available stocks
   */
  void setStock(StockPtrVector& stockvec);
  /**
   * \brief This function will check to see if the age of the stock should be increased on the current timestep or not
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the age of the stock should be increased, 0 otherwise
   */
  int isBirthday(const TimeClass* const TimeInfo) const;
  /**
   * \brief This function will return the flag used to denote whether the stock is eaten or not
   * \return flag
   */
  int isEaten() const { return iseaten; };
  /**
   * \brief This function will return the flag used to denote whether the stock will spawn or not
   * \return flag
   */
  int doesSpawn() const { return doesspawn; };
  /**
   * \brief This function will return the flag used to denote whether the stock will stray or not
   * \return flag
   */
  int doesStray() const { return doesstray; };
  /**
   * \brief This function will return the flag used to denote whether the stock will move or not
   * \return flag
   */
  int doesMove() const { return doesmove; };
  /**
   * \brief This function will return the flag used to denote whether the stock will eat or not
   * \return flag
   */
  int doesEat() const { return doeseat; };
  /**
   * \brief This function will return the flag used to denote whether the stock will mature or not
   * \return flag
   */
  int doesMature() const { return doesmature; };
  /**
   * \brief This function will return the flag used to denote whether the stock will renew or not
   * \return flag
   */
  int doesRenew() const { return doesrenew; };
  /**
   * \brief This function will return the flag used to denote whether the stock will grow or not
   * \return flag
   */
  int doesGrow() const { return doesgrow; };
  /**
   * \brief This function will return the flag used to denote whether the stock will migrate or not
   * \return flag
   */
  int doesMigrate() const { return doesmigrate; };
  /**
   * \brief This function will return the flag used to denote whether the stock has been tagged or not
   * \return flag
   */
  int isTagged() const { return istagged; };
  /**
   * \brief This function will set the flag to denote that the stock has been tagged
   */
  void setTaggedStock() { istagged = 1; };
  /**
   * \brief This function will initialise the tagging experiments for the stock and any related stocks
   */
  void setTagged();
  /**
   * \brief This will return the minimum age of the stock
   * \return minimum age
   */
  int minAge() const { return Alkeys[0].minAge(); };
  /**
   * \brief This will return the maximum age of the stock
   * \return maximum age
   */
  int maxAge() const { return Alkeys[0].maxAge(); };
  /**
   * \brief This will return the total population size (in numbers) of the stock on a given area
   * \param area is the area identifier
   * \return population size
   */
  double getTotalStockNumber(int area) const;
  /**
   * \brief This will return the total population size (in numbers) of the stock across all areas
   * \return population size
   */
  double getTotalStockNumberAllAreas() const;
  /**
   * \brief This will return the total population biomass of the stock on a given area
   * \param area is the area identifier
   * \return population biomass
   */
  double getTotalStockBiomass(int area) const;
  /**
   * \brief This will return the total population biomass of the stock across all areas
   * \return population biomass
   */
  double getTotalStockBiomassAllAreas() const;
  /**
   * \brief This will return the selection weighted population biomass
   * \of the stock in one area return population biomass
   */
  double getWeightedStockBiomass(int area,const FormulaVector & parameters) const;
  /**
   * \brief This will return the selection weighted population biomass
   * \of the stock in allareas return population biomass
   */
  double getWeightedStockBiomassAllAreas(const FormulaVector & parameters) const;
 /**
   * \brief This will return the stocks that this stock will mature in to
   * \return maturestocks, a StockPtrVector of the stocks that this stock will mature into
   */
  const StockPtrVector& getMatureStocks();
  /**
   * \brief This will return the stocks that this stock will move in to
   * \return transitionstocks, a StockPtrVector of the stocks that this stock will move into
   */
  const StockPtrVector& getTransitionStocks();
  /**
   * \brief This will return the stocks that this stock will stray in to
   * \return straystocks, a StockPtrVector of the stocks that this stock will stray into
   */
  const StockPtrVector& getStrayStocks();
  /**
   * \brief This function will add details for a new tagging experiment on the current stock
   * \param tagbyagelength is the AgeBandMatrixPtrVector of the new tagged population
   * \param newtag is the Tags for the new tagging experiment
   * \param tagloss is the proportion of tags that are lost for the new tagging experiment
   */
  void addTags(AgeBandMatrixPtrVector* tagbyagelength, Tags* newtag, double tagloss);
  /**
   * \brief This function will remove a tagging experiment from the current tagged stock population
   * \param tagname is the name of the tagging experiment to be removed
   */
  void deleteTags(const char* tagname);
protected:
  /**
   * \brief This is the AgeBandMatrixPtrVector used to store information about the stock population
   */
  AgeBandMatrixPtrVector Alkeys;
  /**
   * \brief This is the AgeBandMatrixRatioPtrVector used to store information about the tagged population from any tagging experiments performed on the stock
   */
  AgeBandMatrixRatioPtrVector tagAlkeys;
  /**
   * \brief This is the TagPtrVector used to store information about the tagging experiments
   */
  TagPtrVector allTags;
  /**
   * \brief This is the TagPtrVector used to store information about how the tagging experiments affect the population that mature
   */
  TagPtrVector matureTags;
  /**
   * \brief This is the TagPtrVector used to store information about how the tagging experiments affect the population that move
   */
  TagPtrVector transitionTags;
  /**
   * \brief This is the TagPtrVector used to store information about how the tagging experiments affect the population that stray
   */
  TagPtrVector strayTags;
  /**
   * \brief This is the StrayData used to calculate information about the straying of the stock
   */
  StrayData* stray;
  /**
   * \brief This is the SpawnData used to calculate information about the spawning of the stock
   */
  SpawnData* spawner;
  /**
   * \brief This is the RenewalData used to calculate information about the renewal of the stock
   */
  RenewalData* renewal;
  /**
   * \brief This is the Maturity used to calculate information about the maturation of the stock
   */
  Maturity* maturity;
  /**
   * \brief This is the Transition used to calculate information about the movement of the stock
   */
  Transition* transition;
  /**
   * \brief This is the Migration used to calculate information about the migration of the stock
   */
  Migration* migration;
  /**
   * \brief This is the Prey used to calculate information about the consumption of the stock
   */
  StockPrey* prey;
  /**
   * \brief This is the PopPredator used to calculate information about the consumption by the stock
   */
  PopPredator* predator;
  /**
   * \brief This is the InitialCond used to calculate information about the initial conditions of the stock
   */
  InitialCond* initial;
  /**
   * \brief This is the LengthGroupDivision used to store length information
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the Grower used to calculate information about the growth of the stock
   */
  Grower* grower;
  /**
   * \brief This is the NaturalMortality used to calculate information about the natural mortality of the stock
   */
  NaturalMortality* naturalm;
  /**
   * \brief This is the flag used to denote whether the stock will eat (ie. is a Predator) or not
   */
  int doeseat;
  /**
   * \brief This is the flag used to denote whether the stock will move or not
   */
  int doesmove;
  /**
   * \brief This is the flag used to denote whether the stock is eaten (ie. is a Prey) or not
   */
  int iseaten;
  /**
   * \brief This is the flag used to denote whether the stock will spawn or not
   */
  int doesspawn;
  /**
   * \brief This is the flag used to denote whether the stock will mature or not
   */
  int doesmature;
  /**
   * \brief This is the flag used to denote whether the stock will renew or not
   */
  int doesrenew;
  /**
   * \brief This is the flag used to denote whether the stock will grow or not
   */
  int doesgrow;
  /**
   * \brief This is the flag used to denote whether the stock will migrate or not
   */
  int doesmigrate;
  /**
   * \brief This is the flag used to denote whether the stock will stray or not
   */
  int doesstray;
  /**
   * \brief This is the timestep that the population will increase in age in the simulation
   * \note This is currently set to the last timestep in the year
   */
  int birthdate;
  /**
   * \brief This is the flag used to denote whether the stock has been included in a tagging experiment or not
   */
  int istagged;
  /**
   * \brief This is the PopInfoMatrix used to temporarily store the population during the growth calculation
   */
  PopInfoMatrix tmpPopulation;
  /**
   * \brief This is the PopInfoVector used to temporarily store the population during the migration calculation
   */
  PopInfoVector tmpMigrate;
};

#endif
