#ifndef tags_h
#define tags_h

#include "keeper.h"
#include "hasname.h"
#include "commentstream.h"
#include "areatime.h"
#include "charptrvector.h"
#include "stockptrvector.h"
#include "agebandmatrix.h"
#include "stockaggregator.h"
#include "agebandmatrixptrmatrix.h"
#include "livesonareas.h"
#include "doublematrixptrvector.h"
#include "conversionindexptrvector.h"
#include "formula.h"

class Tags : public HasName {
public:
  Tags(CommentStream& infile, const char* givenname, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  ~Tags();
  void Update();
  void Update(int year, int step);
  void UpdateTags(int year, int step);
  void DeleteFromStock();
  const int getTagYear() const { return tagyear; };
  const int getTagStep() const { return tagstep; };
  const int getEndYear() const { return endyear; };
  const int getEndStep() const { return endstep; };
  const int getTagArea() const { return tagarea; };
  const CharPtrVector* getStocknames() const { return &stocknames; };
  void SetStock(StockPtrVector& Stocks);
  const char* TagName() const { return this->Name(); };
  void UpdateMatureStock(const TimeClass* const TimeInfo);
  void UpdateTransitionStock(const TimeClass* const TimeInfo);
  void StoreNumberPriorToEating(int area, const char* stockname);
  const AgeBandMatrix& NumberPriorToEating(int area, const char* stockname);
  void Reset(const TimeClass* const TimeInfo) {};
  int stockIndex(const char* stockname);
private:
  void ReadNumbers(CommentStream& infile, const char* tagname, double minlength, double dl);
  void ReadMultiNumbers(CommentStream& infile, const char* tagname, double minlength,
    double dl, const AreaClass* const Area, const TimeClass* const TimeInfo) {}; //JMB
  void AddToTagStock(int time, int area);
  CharPtrVector stocknames;
  //area-age-length distribution of tags by stocks
  AgeBandMatrixPtrMatrix AgeLengthStock;
  AgeBandMatrixPtrMatrix NumBeforeEating;
  Formula tagloss; //percentage of tags that are lost
  int tagarea;     //area of tagging
  int tagyear;     //year of tagging
  int tagstep;     //step of tagging
  int endyear;     //year of last recapture
  int endstep;     //step of last recapture
  DoubleVector NumberByLength;
  DoubleMatrixPtrVector NumberByLengthMulti;
  LengthGroupDivision* LgrpDiv;
  StockPtrVector tagstocks;
  StockPtrVector maturestocks;
  StockPtrVector transitionstocks;
  ConversionIndexPtrVector CI;
  Stock* taggingstock;
  IntVector preyindex;
  IntVector updated;
  IntVector areaindex;
  IntVector Years;
  IntVector Steps;
  IntMatrix Areas;
  int multiTags;
};
#endif
