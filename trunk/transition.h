#ifndef transition_h
#define transition_h

#include "areatime.h"
#include "formulavector.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "agebandmatrixptrvector.h"
#include "agebandmatrixratioptrvector.h"
#include "stock.h"

/**
 * \class Transition
 * \brief This is the class used to calculate the movement from a young stock to older stocks
 *
 * This class moves the oldest age group of a young stock up to older stocks.  This is a simpler method than the Maturity classes of moving fish between stocks, and is used to move the fish that haven't matured according to the maturation functions.  The length groups of the older stocks are checked, and any fish that haven't yet reached the minimum length of the older stock remain in the younger stock, effectively as an age-plus group.
 */
class Transition : public HasName, protected LivesOnAreas {
public:
  /**
   * \brief This is the Transition constructor
   * \param infile is the CommentStream to read the transition data from
   * \param areas is the IntVector of areas that the movements will be calculated on
   * \param age is the maximum age for the (young) stock
   * \param lgrpdiv is the LengthGroupDivision for the stock
   * \param givenname is the name of the stock for this Transition class
   * \param TimeInfo is the TimeClass for the current model
   * \param keeper is the Keeper for the current model
   */
  Transition(CommentStream& infile, const IntVector& areas, int age,
    const LengthGroupDivision* const lgrpdiv, const char* givenname,
    const TimeClass* const TimeInfo, Keeper* const keeper);
  /**
   * \brief This is the default Transition destructor
   */
  ~Transition();
  /**
   * \brief This will select the stocks required for the movement between stocks
   * \param stockvec is the StockPtrVector of all the available stocks
   */
  void setStock(StockPtrVector& stockvec);
  /**
   * \brief This will store the part of the younger stock that will move into the older stocks
   * \param area is the area that the transition is being calculated on
   * \param Alkeys is the AgeBandMatrix of the stock that fish will move from
   * \param TimeInfo is the TimeClass for the current model
   */
  void storeTransitionStock(int area, AgeBandMatrix& Alkeys, const TimeClass* const TimeInfo);
  /**
   * \brief This will store the part of the younger stock that will move into the older stocks
   * \param area is the area that the transition is being calculated on
   * \param Alkeys is the AgeBandMatrix of the stock that fish will move from
   * \param TagAlkeys is the AgeBandMatrixRatio of the tagged stock that fish will move from
   * \param TimeInfo is the TimeClass for the current model
   */
  void storeTransitionStock(int area, AgeBandMatrix& Alkeys,
    AgeBandMatrixRatio& TagAlkeys, const TimeClass* const TimeInfo);
  /**
   * \brief This will move the younger stock into the older stock age-length cells
   * \param area is the area that the movement is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   */
  void Move(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will print the transition information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile) const;
  /**
   * \brief This will calculate the stocks the young stock can move into
   * \return transition stocks
   */
  const StockPtrVector& getTransitionStocks();
  /**
   * \brief This function will initialise the tagging experiments for the transition calculations
   */
  void setTagged();
  /**
   * \brief This will add a tagging experiment to the transition calculations
   * \param tagname is the name of the tagging experiment
   */
  void addTransitionTag(const char* tagname);
  /**
   * \brief This will remove a tagging experiment from the transition calculations
   * \param tagname is the name of the tagging experiment
   */
  void deleteTransitionTag(const char* tagname);
  /**
   * \brief This function will reset the transition data
   */
  void Reset();
  /**
   * \brief This will check if the transition process will take place on the current timestep
   * \param TimeInfo is the TimeClass for the current model
   * \return 1 if the transition process will take place, 0 otherwise
   */
  virtual int isTransitionStep(const TimeClass* const TimeInfo);
protected:
  /**
   * \brief This is the StockPtrVector of the stocks that the young stock will move to
   */
  StockPtrVector transitionStocks;
  /**
   * \brief This is the CharPtrVector of the names of the transition stocks
   */
  CharPtrVector transitionStockNames;
  /**
   * \brief This is the FormulaVector of the ratio of the young stock to move into each older stock
   */
  FormulaVector transitionRatio;
  /**
   * \brief This is used to scale the ratios to ensure that they will always sum to 1
   */
  double ratioscale;
  /**
   * \brief This is the IntVector used as an index for the ratio vector
   */
  IntVector ratioindex;
  /**
   * \brief This is the ConversionIndexPtrVector used to convert from the young stock lengths to the old stock lengths
   */
  ConversionIndexPtrVector CI;
  /**
   * \brief This is the LengthGroupDivision of the stock
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the timestep that the movement between stocks will occur on
   */
  int transitionStep;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to store the calculated old stocks
   */
  AgeBandMatrixPtrVector Storage;
  /**
   * \brief This is the AgeBandMatrixRatioPtrVector used to store the calculated tagged stocks
   */
  AgeBandMatrixRatioPtrVector tagStorage;
  /**
   * \brief This is the age that the younger stock will move into the older stocks
   */
  int age;
  /**
   * \brief This is the minimum length group of the younger stock that exists in the older stock
   */
  int minTransitionLength;
  /**
   * \brief This is the flag used to denote whether the stock has been included in a tagging experiment or not
   */
  int istagged;
};

#endif
