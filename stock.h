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
#include "naturalm.h"

class Stock;
class Keeper;
class NaturalM;
class Grower;
class InitialCond;
class Spawner;
class Alkeys;
class Migration;
class Maturity;
class Transition;
class StockPredator;
class Predator;
class PopPredator;
class StockPrey;
class Prey;
class Maturity;
class RenewalData;
class Spawner;

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
  virtual void CalcNumbers(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will reduce the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void ReducePop(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
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
  virtual void FirstUpdate(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the age increase of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void SecondUpdate(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will implement the transiton of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void ThirdUpdate(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the spawning of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void FirstSpecialTransactions(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the maturity and recruits of the model population for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void SecondSpecialTransactions(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will calculate the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void CalcEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will check the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void CheckEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will adjust the modelled consumption for an area in the model
   * \param area is an integer to denote the internal area of interest
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void AdjustEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo);
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
   * \brief This function will recalculate the migration of the model population for the model
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void RecalcMigration(const TimeClass* const TimeInfo);
  virtual void Clear();
  void Renewal(int area, const TimeClass* const TimeInfo);
  void Add(const AgeBandMatrix& Addition, const ConversionIndex* const CI,
    int area, double ratio = 1, int MinAge = 0, int MaxAge = 100);
  void Add(const AgeBandMatrixRatioPtrVector& Addition, int AddArea, const ConversionIndex* const CI,
    int area, double ratio = 1, int MinAge = 0, int MaxAge = 100);
  Prey* ReturnPrey() const;
  const Migration* ReturnMigration() const { return migration; };
  PopPredator* ReturnPredator() const;
  const AgeBandMatrix& Agelengthkeys(int area) const;
  const AgeBandMatrix& getMeanN(int area) const;
  virtual void SetStock(StockPtrVector& stockvec);
  void SetCI();
  /**
   * \brief This function will print the model population
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile) const;
  int Birthday(const TimeClass* const TimeInfo) const;
  const LengthGroupDivision* ReturnLengthGroupDiv() const { return LgrpDiv; };
  int IsEaten() const { return iseaten; };
  int DoesSpawn() const { return doesspawn; };
  int DoesMove() const { return doesmove; };
  int DoesEat() const { return doeseat; };
  int DoesMature() const { return doesmature; };
  int DoesRenew() const { return doesrenew; };
  int DoesGrow() const { return doesgrow; };
  int DoesMigrate() const { return doesmigrate; };
  int Minage() const { return Alkeys[0].Minage(); };
  int Maxage() const { return Alkeys[0].Maxage(); };
  const DoubleIndexVector& mortality() const { return NatM->getMortality(); };
  //const void getBiomass(int area) const {};
  const StockPtrVector& GetMatureStocks();
  const StockPtrVector& GetTransitionStocks();
  void UpdateTags(AgeBandMatrixPtrVector* tagbyagelength, Tags* newtag, double tagloss);
  void DeleteTags(const char* tagname);
  void UpdateMatureStockWithTags(const TimeClass* const TimeInfo);
  void UpdateTransitionStockWithTags(const TimeClass* const TimeInfo);
  const CharPtrVector TaggingExperimentIds();
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
  Spawner* spawner;
  RenewalData* renewal;
  Maturity* maturity;
  Transition* transition;
  int AgeDepMigration;
  Migration* migration;
  Prey* prey;
  PopPredator* predator;
  InitialCond* initial;
  LengthGroupDivision* LgrpDiv;
  Grower* grower;
  NaturalM* NatM;
  PopInfoMatrix NumberInArea;
  int doeseat;
  int doesmove;
  int iseaten;
  int doesspawn;
  int doesmature;
  int doesrenew;
  int doesgrow;
  int doesmigrate;
  int birthdate;
  /**
   * \brief This denotes what type of stock class has been created
   */
  StockType type;
};

#endif
