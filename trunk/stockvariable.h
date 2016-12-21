#ifndef stockvariable_h
#define stockvariable_h

#include "formulavector.h"
#include "areatime.h"
#include "commentstream.h"
#include "charptrvector.h"
#include "stockptrvector.h"

/**
 * \class StockVariable
 * \brief This is the class used to calculate the value of the stock-dependent variables used in the model simulation
 */
class StockVariable {
public:
  /**
   * \brief This is the default StockVariable constructor
   */
  StockVariable() { value = 0.0; };
  /**
   * \brief This is the default StockVariable destructor
   */
  ~StockVariable();
  /**
   * \brief This function will read StockVariable data from file
   * \param infile is the CommentStream to read the data from
   */
  void read(CommentStream& infile);
  /**
   * \brief This function will return the value of the StockVariable
   * \return the value of the StockVariable
   */
  double getValue() const { return value; };
  /**
   * \brief This function will swap the StockVariable value for a new value
   * \param newSV is the new StockVariable value
   */
  void Interchange(StockVariable& newSV) const;
  /**
   * \brief This function will update the StockVariable value
   */
  void Update();
  /**
   * \brief This function will delete a StockVariable value
   */
  void Delete() const;
private:
  /**
   * \brief This is the value of the StockVariable
   */
  double value;
  /**
   * \brief This is the flag to denote whether the stock calculations should be based on the biomass of the stocks, or the population number
   * \note The default value is 1, which calculates the value based on the biomass
   */
  int biomass;
  /**
   * \brief This is the CharPtrVector of the names of the stocks that will be used to calculate the StockVariable value
   */
  CharPtrVector stocknames;
  /**
   * \brief This is the StockPtrVector of the stocks that will be used to calculate the StockVariable value
   */
  StockPtrVector stocks;
};

#endif
