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
  /**
   * \brief This is the StrayData constructor
   * \param infile is the CommentStream to read the straying data from
   * \param LgrpDiv is the LengthGroupDivision that the straying stock will be calculated on
   * \param areas is the IntVector of areas that the straying stock will be calculated on
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  StrayData(CommentStream& infile, const LengthGroupDivision* const LgrpDiv,
    const IntVector& areas, const AreaClass* const Area,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default StrayData destructor
   */
  ~StrayData();
  /**
   * \brief This function will print the straying information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile) const;
  /**
   * \brief This will select the stocks required for the straying process
   * \param stockvec is the StockPtrVector of all the available stocks
   */
  void setStock(StockPtrVector& stockvec);
  /**
   * \brief This will move the calculated straying stock from the current stock into the new stock age-length cells
   * \param area is the area that the straying process is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   */
  void addStrayStock(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This will store the part of the current stock that will stray into other stocks
   * \param area is the area that the straying is being calculated on
   * \param Alkeys is the AgeBandMatrix of the stock that fish will move from
   * \param TagAlkeys is the AgeBandMatrixRatio of the tagged stock that fish will move from
   * \param TimeInfo is the TimeClass for the current model
   */
  void storeStrayingStock(int area, AgeBandMatrix& Alkeys,
    AgeBandMatrixRatio& TagAlkeys,const TimeClass* const TimeInfo);
  /**
   * \brief This will check if the straying process will take place on the current timestep
   * \param area is the area that the straying is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the straying process will take place, 0 otherwise
   */
  int isStrayStepArea(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This will reset the straying information for the current model run
   * \param TimeInfo is the TimeClass for the current model
   */
  void Reset(const TimeClass* const TimeInfo);
  /**
   * \brief This will calculate the stocks the current stock can stray into
   * \return stray stocks
   */
  const StockPtrVector& getStrayStocks();
  /**
   * \brief This will add a tagging experiment to the straying calculations
   * \param tagname is the name of the tagging experiment
   */
  void addStrayTag(const char* tagname);
  /**
   * \brief This will remove a tagging experiment from the straying calculations
   * \param tagname is the name of the tagging experiment
   */
  void deleteStrayTag(const char* tagname);
protected:
  /**
   * \brief This is the StockPtrVector of the stocks that the straying fish will move to
   */
  StockPtrVector strayStocks;
  /**
   * \brief This is the CharPtrVector of the names of the straying stocks
   */
  CharPtrVector strayStockNames;
  /**
   * \brief This is the DoubleVector of the ratio of the stock that will stray
   */
  DoubleVector Ratio;
  /**
   * \brief This is the IntVector of timesteps that the straying will take place on
   */
  IntVector strayStep;
  /**
   * \brief This is the IntVector of areas that the straying will take place on
   */
  IntVector strayArea;
  /**
   * \brief This is the LengthGroupDivision of the stock
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the DoubleVector of the proportion of each length group of the stock that will stray
   */
  DoubleVector strayProportion;
  /**
   * \brief This is the SelectFunc used to calculate of the proportion of each length group of the stock that will stray
   */
  SelectFunc* fnProportion;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to store the calculated straying stocks
   */
  AgeBandMatrixPtrVector Storage;
  /**
   * \brief This is the AgeBandMatrixRatioPtrVector used to store the calculated straying tagged stocks
   */
  AgeBandMatrixRatioPtrVector TagStorage;
  /**
   * \brief This is the ConversionIndexPtrVector used to convert from the current stock lengths to the straying stock lengths
   */
  ConversionIndexPtrVector CI;
  /**
   * \brief This is the minimum length group division of the straying stock
   */
  int minStrayLength;
};

#endif
