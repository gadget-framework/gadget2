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
class StockRecruitment;
class Maturity;
class RenewalData;
class Spawner;

enum StockType { STOCKTYPE, LENSTOCKTYPE };

class Stock : public BaseClass {
public:
  Stock(CommentStream& infile, const char* givenname, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  Stock(const char* givenname);
  virtual ~Stock();
  virtual StockType stockType() const { return STOCKTYPE; };
  virtual void CalcNumbers(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void ReducePop(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void Grow(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void FirstUpdate(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void SecondUpdate(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void ThirdUpdate(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void FirstSpecialTransactions(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void SecondSpecialTransactions(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  void Renewal(int area, const TimeClass* const TimeInfo, double ratio = 0);
  void Add(const AgeBandMatrix& Addition, const ConversionIndex* const CI,
    int area, double ratio = 1, int MinAge = 0, int MaxAge = 100);
  void Add(const AgeBandMatrixRatioPtrVector& Addition, int AddArea, const ConversionIndex* const CI,
    int area, double ratio = 1, int MinAge = 0, int MaxAge = 100);
  virtual void Migrate(const TimeClass* const TimeInfo);
  virtual void CalcEat(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void CheckEat(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void AdjustEat(int area,
    const AreaClass* const Area, const TimeClass* const TimeInfo);
  virtual void Reset(const TimeClass* const TimeInfo);
  virtual void RecalcMigration(const TimeClass* const TimeInfo);
  virtual void Clear();
  Prey* ReturnPrey() const;
  const Migration* ReturnMigration() const { return migration; };
  PopPredator* ReturnPredator() const;
  const AgeBandMatrix& Agelengthkeys(int area) const;
  const AgeBandMatrix& getMeanN(int area) const;
  virtual void SetStock(StockPtrVector& stockvec);
  void SetCI();
  void Print(ofstream& outfile) const;
  int Birthday(const TimeClass* const TimeInfo) const;
  const LengthGroupDivision* ReturnLengthGroupDiv() const { return LgrpDiv; };
  int IsEaten() const { return iseaten; };
  int DoesSpawn() const { return doesspawn; };
  int DoesMove() const { return doesmove; };
  int DoesEat() const { return doeseat; };
  int DoesMature() const { return doesmature; };
  int HasLgr() const { return haslgr; };
  int DoesRenew() const { return doesrenew; };
  int DoesGrow() const { return doesgrow; };
  int DoesMigrate() const { return doesmigrate; };
  int Minage() const { return Alkeys[0].Minage(); };
  int Maxage() const { return Alkeys[0].Maxage(); };
  const DoubleIndexVector& mortality() const { return NatM->getMortality(); };
  const void getBiomass(int area) const {};
  const StockPtrVector& GetMatureStocks();
  int UpdateTags(AgeBandMatrixPtrVector* tagbyagelength, Tags* newtag);
  void DeleteTags(const char* tagname);
  void UpdateMatureStockWithTags(const TimeClass* const TimeInfo);
  const CharPtrVector TaggingExperimentIds();
protected:
  AgeBandMatrixPtrVector Alkeys;
  AgeBandMatrixRatioPtrVector tagAlkeys;
  TagPtrVector matureTags;
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
  int haslgr;
  int doesmature;
  int doesrenew;
  int doesgrow;
  int doesmigrate;
  int birthdate;
};

#endif
