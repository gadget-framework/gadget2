#ifndef tags_h
#define tags_h

#include "keeper.h"
#include "hasname.h"
#include "commentstream.h"
#include "areatime.h"
#include "charptrvector.h"
#include "stockptrvector.h"
#include "agebandmatrix.h"
#include "agebandmatrixptrmatrix.h"
#include "livesonareas.h"
#include "doublematrixptrvector.h"
#include "conversionindexptrvector.h"
#include "formula.h"

class Tags : public HasName {
public:
  Tags(CommentStream& infile, const char* givenname, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper, StockPtrVector stockvec);
  ~Tags();
  void Update(int timeid);
  void updateTags(int year, int step);
  void deleteStockTags();
  const int getEndYear() const { return endyear; };
  int getNumTagTimeSteps() const { return numtagtimesteps; };
  const CharPtrVector* getStockNames() const { return &stocknames; };
  void setStock(StockPtrVector& Stocks);
  void updateMatureStock(const TimeClass* const TimeInfo);
  void updateTransitionStock(const TimeClass* const TimeInfo);
  void updateStrayStock(const TimeClass* const TimeInfo);
  void storeNumberPriorToEating(int area, const char* stockname);
  const AgeBandMatrix& getNumberPriorToEating(int area, const char* stockname);
  void Reset(const TimeClass* const TimeInfo);
  int isWithinPeriod(int year, int step);
  int stockIndex(const char* stockname);
  int areaIndex(const char* stockname, int area);
  void addToTagStock(int timeid);
private:
  void readNumbers(CommentStream& infile, const char* tagname, const TimeClass* const TimeInfo);
  CharPtrVector stocknames;
  //area-age-length distribution of tags by stocks
  AgeBandMatrixPtrMatrix AgeLengthStock;
  AgeBandMatrixPtrMatrix NumBeforeEating;
  Formula tagloss; //percentage of tags that are lost
  int tagarea;     //area of tagging
  int tagyear;     //year of tagging
  int tagstep;     //step of tagging
  int endyear;     //year of last recapture
  int numtagtimesteps;
  DoubleMatrixPtrVector NumberByLength;
  LengthGroupDivision* LgrpDiv;
  StockPtrVector tagStocks;
  StockPtrVector matureStocks;
  StockPtrVector transitionStocks;
  StockPtrVector strayStocks;
  ConversionIndexPtrVector CI;
  Stock* taggingstock;
  IntVector preyindex;
  IntVector updated;
  IntVector Years;
  IntVector Steps;
};
#endif
