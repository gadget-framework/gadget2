#ifndef stock_h
#define stock_h

#include "agebandm.h"
#include "agebandmatrixratio.h"
#include "base.h"
#include "stockptrvector.h"
#include "catchdataptrvector.h"
#include "tagptrvector.h"
#include "commentstream.h"
#include "naturalm.h"

class Stock;
class Keeper;
class AreaClass;
class TimeInfo;
class NaturalM;
class Grower;
class InitialCond;
class Spawner;
class Alkeys;
class Migration;
class Maturity;
class CatchData;
class Transition;
class StockPredator;
class Predator;
class PopPredator;
class StockPrey;
class Prey;
class Catch;
class StockRecruitment;
class Maturity;
class RenewalData;
class Spawner;
class LengthGroupDivision;
class ConversionIndex;

enum stock_type { STOCK_TYPE, LENSTOCK_TYPE };

class Stock : public BaseClass {
public:
  Stock(CommentStream& infile, const char* givenname, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  Stock(const char* givenname);
  virtual ~Stock();
  virtual stock_type stockType() const { return STOCK_TYPE; };
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
  void Add(const Agebandmatrix& Addition, const ConversionIndex* const CI,
    int area, double ratio = 1, int MinAge = 0, int MaxAge = 100);
  void Add(const Agebandmatrixratiovector& Addition, int AddArea, const ConversionIndex* const CI,
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
  const Agebandmatrix& Agelengthkeys(int area) const;
  Agebandmatrix& MutableAgelengthkeys(int area) const;
  const Agebandmatrix& getMeanN(int area) const;
  virtual void SetStock(Stockptrvector& stockvec);
  void SetCatch(CatchDataptrvector& CDvector);
  void SetCI();
  void Print(ofstream& outfile) const;
  int Birthday(const TimeClass* const TimeInfo) const;
  const LengthGroupDivision* ReturnLengthGroupDiv() const { return LgrpDiv; };
  int IsEaten() const { return iseaten; };
  int IsCaught() const { return iscaught; };
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
  const doubleindexvector& mortality() const { return NatM->getMortality(); };
  const void getBiomass(int area) const {};
  const Stockptrvector& GetMatureStocks();
  int UpdateTags(Agebandmatrixvector* tagbyagelength, Tags* newtag);
  void DeleteTags(const char* tagname);
  void UpdateMatureStockWithTags(const TimeClass* const TimeInfo);
  const charptrvector TaggingExperimentIds();
protected:
  Agebandmatrixvector Alkeys;
  Agebandmatrixratiovector tagAlkeys;
  Tagptrvector matureTags;
  Spawner* spawner;
  Catch* catchptr;
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
  popinfomatrix NumberInArea;
  int iscaught;
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
