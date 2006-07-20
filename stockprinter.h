#ifndef stockprinter_h
#define stockprinter_h

#include "printer.h"
#include "stockaggregator.h"

/**
 * \class StockPrinter
 * \brief This is the class used to print information about one or more stocks
 */
class StockPrinter : public Printer {
public:
  /**
   * \brief This is the default StockPrinter constructor
   * \param infile is the CommentStream to read the printer parameters from
   * \param TimeInfo is the TimeClass for the current model
   */
  StockPrinter(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default StockPrinter destructor
   */
  virtual ~StockPrinter();
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
   * \brief This is the IntMatrix used to store aggregated area information
   */
  IntMatrix areas;
  /**
   * \brief This is the IntMatrix used to store aggregated age information
   */
  IntMatrix ages;
  /**
   * \brief This is the CharPtrVector of the names of the aggregated areas to print
   */
  CharPtrVector areaindex;
  /**
   * \brief This is the CharPtrVector of the names of the aggregated ages to print
   */
  CharPtrVector ageindex;
  /**
   * \brief This is the CharPtrVector of the names of the lengths to print
   */
  CharPtrVector lenindex;
  /**
   * \brief This is the CharPtrVector of the names of the stocks to print
   */
  CharPtrVector stocknames;
  /**
   * \brief This is the LengthGroupDivision of the stock
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the StockAggregator used to collect information about the stocks
   */
  StockAggregator* aggregator;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to temporarily store the information returned from the aggregatation function
   */
  const AgeBandMatrixPtrVector* alptr;
};

#endif
