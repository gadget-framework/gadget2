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
#include "formula.h"

class Tags : public HasName {
public:
  Tags(CommentStream& infile, const char* givenname, const AreaClass* const Area,
       const TimeClass* const TimeInfo, Keeper* const keeper);
  ~Tags();
  void Update();
  void DeleteFromStock();
  const int getTagYear() const { return tagyear; };
  const int getTagStep() const { return tagstep; };
  const int getEndYear() const { return endyear; };
  const int getEndStep() const { return endstep; };
  const int getTagArea() const { return tagarea; };
  CharPtrVector getStocknames() const { return stocknames; };
  void SetStock(StockPtrVector& Stocks);
  const char* TagName() const { return this->Name(); };
  void UpdateMatureStock(const TimeClass* const TimeInfo);
  void printPopInfo(char* filename);
private:
  void ReadNumbers(CommentStream&, const char*, double, double dl);
  CharPtrVector stocknames;   //names of the tagged stock read from file
  //area-age-length distribution of tags by stocks
  AgeBandMatrixPtrMatrix AgeLengthStock;
  //AgeLengthStock[0] refers to the stock and AgeLengthStock[...] refers to the mature stock.
  Formula tagloss; //percentage of tags that are lost
  int tagarea;     //area of tagging
  int tagyear;     //year of tagging
  int tagstep;     //step of tagging
  int endyear;     //year of last recapture
  int endstep;     //step of last recapture
  DoubleVector NumberByLength;
  LengthGroupDivision* LgrpDiv;
  StockPtrVector tagstocks;
  StockPtrVector maturestocks;
};
#endif
