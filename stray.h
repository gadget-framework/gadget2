#ifndef stray_h
#define stray_h

#include "areatime.h"
#include "timevariablevector.h"
#include "commentstream.h"
#include "agebandmatrix.h"
#include "intmatrix.h"
#include "livesonareas.h"
#include "selectfunc.h"
#include "stock.h"

class StrayData : protected LivesOnAreas {
public:
  StrayData(CommentStream& infile, const LengthGroupDivision* const LgrpDiv,
    const IntVector& Areas, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  ~StrayData();
  void Print(ofstream& outfile) const;
  void setStock(StockPtrVector& stockvec);
  void addStrayStock(int area, const TimeClass* const TimeInfo);
  void storeStrayingStock(int area, AgeBandMatrix& Alkeys,
    AgeBandMatrixRatio& TagAlkeys,const TimeClass* const TimeInfo);
  int IsStrayStepArea(int area, const TimeClass* const TimeInfo);
  void Reset(const TimeClass* const TimeInfo);
  const StockPtrVector& getStrayStocks();
  void addStrayTag(const char* tagname);
  void deleteStrayTag(const char* tagname);
protected:
  StockPtrVector strayStocks;
  CharPtrVector strayStockNames;
  DoubleVector Ratio;
  IntVector straystep;
  IntVector strayarea;
  LengthGroupDivision* LgrpDiv;
  DoubleVector strayProportion;
  SelectFunc* fnProportion;
  AgeBandMatrixPtrVector Storage;
  AgeBandMatrixRatioPtrVector TagStorage;
  ConversionIndexPtrVector CI;
  int minStrayLength;
};

#endif
