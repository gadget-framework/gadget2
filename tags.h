#ifndef tags_h
#define tags_h

#include "keeper.h"
#include "hasname.h"
#include "commentstream.h"
#include "areatime.h"
#include "charptrvector.h"
#include "stockptrvector.h"
#include "agebandm.h"
#include "stockaggregator.h"
#include "agebandmatrixptrvector.h"
#include "livesonareas.h"
#include "formula.h"

class Tags : public HasName {
public:
  Tags(CommentStream& infile, const char* givenname, const AreaClass* const Area,
       const TimeClass* const TimeInfo, Keeper* const keeper);
  ~Tags();
  void Update();
  void DeleteFromStock();
  const int getTagYear () const { return tagyear; };
  const int getTagStep () const { return tagstep; };
  const int getEndYear () const { return endyear; };
  const int getEndStep () const { return endstep; };
  const int getTagArea () const { return tagarea; };
  charptrvector getStocknames() const { return stocknames; };
  void SetStock(Stockptrvector& Stocks);
  const char* TagName() const { return this->Name(); };
  void printPopInfo(char* filename);
private:
  void ReadNumbers(CommentStream&, const char*, double);
  //char* tagname;  //name of the tagging experiment
  charptrvector stocknames;   //names of the tagged stock read from file
  //age-length distribution of tags by stocks
  agebandmatrixptrvector AgeLengthStock;
  agebandmatrixptrvector matureStock;
  Formula tagloss; //percentage of tags that are lost
  int tagarea;     //area of tagging
  int tagyear;     //year of tagging
  int tagstep;     //step of tagging
  int endyear;     //year of last recapture
  int endstep;     //step of last recapture
  doublevector NumberByLength;
  LengthGroupDivision* LgrpDiv;
  Stockptrvector tagstocks;
  Stockptrvector maturestocks;
  StockAggregator* aggregator;
};
#endif
