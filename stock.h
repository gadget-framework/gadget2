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
class NaturalM;
class Grower;
class InitialCond;
class Migration;
class Transition;
class PopPredator;
class Prey;
class Maturity;
class RenewalData;
class SpawnData;
class StrayData;

enum StockType { STOCKTYPE = 1, LENSTOCKTYPE };

/**
 * \class Stock
 * \brief This is the class used to model a stock within the Gadget model
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
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void calcNumbers(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will reduce the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void reducePop(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
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
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updateAgePart1(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the age increase of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updateAgePart2(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will implement the transiton of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updateAgePart3(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the spawning of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart1(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate add the newly matured stock into the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart2(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate add the new recruits into the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart3(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate calculate the straying of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart4(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate add the strayed stock into the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void updatePopulationPart5(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
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
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void checkEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will adjust the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void adjustEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will reset the model population
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Reset(const TimeClass* const TimeInfo);
  /**
   * \brief This function will implement the migration of the model population for the model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Migrate(const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the migration of the model population for the model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void calcMigration(const TimeClass* const TimeInfo);
  void Renewal(int area, const TimeClass* const TimeInfo);
  void Add(const AgeBandMatrix& Addition, const ConversionIndex* const CI,
    int area, double ratio = 1.0, int MinAge = 0, int MaxAge = 100);
  void Add(const AgeBandMatrixRatioPtrVector& Addition, int AddArea, const ConversionIndex* const CI,
    int area, double ratio = 1.0, int MinAge = 0, int MaxAge = 100);
  Prey* returnPrey() const;
  const Migration* returnMigration() const { return migration; };
  PopPredator* returnPredator() const;
  const AgeBandMatrix& getAgeLengthKeys(int area) const;
  virtual void setStock(StockPtrVector& stockvec);
  void setCI();
  /**
   * \brief This function will print the model population
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile) const;
  int Birthday(const TimeClass* const TimeInfo) const;
  const LengthGroupDivision* returnLengthGroupDiv() const { return LgrpDiv; };
  int isEaten() const { return iseaten; };
  int doesSpawn() const { return doesspawn; };
  int doesStray() const { return doesstray; };
  int doesMove() const { return doesmove; };
  int doesEat() const { return doeseat; };
  int doesMature() const { return doesmature; };
  int doesRenew() const { return doesrenew; };
  int doesGrow() const { return doesgrow; };
  int doesMigrate() const { return doesmigrate; };
  int minAge() const { return Alkeys[0].minAge(); };
  int maxAge() const { return Alkeys[0].maxAge(); };
  const StockPtrVector& getMatureStocks();
  const StockPtrVector& getTransitionStocks();
  const StockPtrVector& getStrayStocks();
  void updateTags(AgeBandMatrixPtrVector* tagbyagelength, Tags* newtag, double tagloss);
  void deleteTags(const char* tagname);
  void updateMatureStockWithTags(const TimeClass* const TimeInfo);
  void updateTransitionStockWithTags(const TimeClass* const TimeInfo);
  void updateStrayStockWithTags(const TimeClass* const TimeInfo);
  /**
   * \brief This will return the type of stock class
   * \return type
   */
  StockType Type() { return type; };
protected:
  AgeBandMatrixPtrVector Alkeys;
  AgeBandMatrixRatioPtrVector tagAlkeys;
  TagPtrVector allTags;
  TagPtrVector matureTags;
  TagPtrVector transitionTags;
  TagPtrVector strayTags;
  StrayData* stray;
  SpawnData* spawner;
  RenewalData* renewal;
  Maturity* maturity;
  Transition* transition;
  Migration* migration;
  Prey* prey;
  PopPredator* predator;
  InitialCond* initial;
  LengthGroupDivision* LgrpDiv;
  Grower* grower;
  NaturalM* naturalm;
  PopInfoMatrix NumberInArea;
  int doeseat;
  int doesmove;
  int iseaten;
  int doesspawn;
  int doesmature;
  int doesrenew;
  int doesgrow;
  int doesmigrate;
  int doesstray;
  int birthdate;
  /**
   * \brief This denotes what type of stock class has been created
   */
  StockType type;
};

#endif
