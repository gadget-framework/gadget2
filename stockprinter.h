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
   * \param Area is the AreaClass for the current model
   * \param TimeInfo is the TimeClass for the current model
   */
  StockPrinter(CommentStream& infile, const AreaClass* const Area, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default StockPrinter destructor
   */
  virtual ~StockPrinter();
  /**
   * \brief This will select the stocks required for the printer class to print the requested information
   * \param stockvec is the StockPtrVector of all the available stocks
   */
  virtual void setStock(StockPtrVector& stockvec);
  /**
   * \brief This will print the requested information for the printer class to the ofstream specified
   * \param TimeInfo is the TimeClass for the current model
   */
  virtual void Print(const TimeClass* const TimeInfo);
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
   * \brief This ofstream is the file that all the model information gets sent to
   */
  ofstream outfile;
};

#endif
