#ifndef catch_h
#define catch_h

#include "areatime.h"
#include "stockptrvector.h"
#include "bandmatrixptrmatrix.h"
#include "agebandm.h"
#include "livesonareas.h"
#include "hasname.h"

class CatchData;
class Catch;
class Stock;
class ConversionIndex;

class CatchData : protected LivesOnAreas, protected HasName {
public:
  CatchData(CommentStream& infile, const char* givenname, const AreaClass* const Area, const TimeClass* const TimeInfo);
  ~CatchData();
  int FindStock(const Stock* stock);
  const LengthGroupDivision* ReturnLengthGroupDiv() const { return LgrpDiv; };
  const bandmatrix* GetCatch(int area, const TimeClass* const TimeInfo) const;
  void AddStock(Stock& stock);
  void Recalc(int area, const TimeClass* const TimeInfo);
  const bandmatrix* GetCatch(Stock& stock);
protected:
  int stockIndex(Stock& stock);
  LengthGroupDivision* LgrpDiv;
  bandmatrixptrmatrix Catchmatrix;
  int numberofstocks;
  Stockptrvector stocks;
  bandmatrixptrvector stockCatch;
  const Agebandmatrix* total;
  bandmatrixptrvector coarseN;
};

class Catch {
public:
  Catch(CatchData* CD);
  ~Catch();
  void Subtract(Agebandmatrix& Alkeys, int area, const TimeClass* const TimeInfo);
  void Subtract(Stock& stock, int area);
  const bandmatrix* GetCatch(int area, const TimeClass* const TimeInfo) const;
  void SetCI(const LengthGroupDivision* const GivenLDiv);
protected:
  ConversionIndex* CI;
  CatchData* Catchdata;
};

#endif
