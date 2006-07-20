#ifndef stockfullprinter_h
#define stockfullprinter_h

#include "printer.h"
#include "stockaggregator.h"

/**
 * \class StockFullPrinter
 * \brief This is the class used to print detailed information about a stock
 */
class StockFullPrinter : public Printer {
public:
  /**
   * \brief This is the default StockFullPrinter constructor
   * \param infile is the CommentStream to read the printer parameters from
   * \param TimeInfo is the TimeClass for the current model
   */
  StockFullPrinter(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default StockFullPrinter destructor
   */
  virtual ~StockFullPrinter();
  /**
   * \brief This will select the stocks required for the printer class to print the requested information
   * \param stockvec is the StockPtrVector of all the available stocks
   * \param Area is the AreaClass for the current model
   */
  virtual void setStock(StockPtrVector& stockvec, const AreaClass* const Area);
  /**
   * \brief This will print the requested information for the printer class to the ofstream specified
   * \param TimeInfo is the TimeClass for the current model
   * \param printtime is the flag to denote when the printing is taking place
   */
  virtual void Print(const TimeClass* const TimeInfo, int printtime);
protected:
  /**
   * \brief This is the IntVector used to store aggregated area information
   */
  IntVector outerareas;
  /**
   * \brief This is the minimum age of the stock
   */
  int minage;
  /**
   * \brief This is the name of the stock
   */
  char* stockname;
  /**
   * \brief This is the StockAggregator used to collect information about the stock
   */
  StockAggregator* aggregator;
  /**
   * \brief This is the LengthGroupDivision of the stock
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to temporarily store the information returned from the aggregatation function
   */
  const AgeBandMatrixPtrVector* alptr;
};

#endif
