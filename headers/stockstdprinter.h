#ifndef stockstdprinter_h
#define stockstdprinter_h

#include "printer.h"
#include "stockaggregator.h"
#include "stockpreyaggregator.h"
#include "popstatistics.h"

/**
 * \class StockStdPrinter
 * \brief This is the class used to print 'standard' information about a stock
 */
class StockStdPrinter : public Printer {
public:
  /**
   * \brief This is the default StockStdPrinter constructor
   * \param infile is the CommentStream to read the printer parameters from
   * \param TimeInfo is the TimeClass for the current model
   */
  StockStdPrinter(CommentStream& infile, const TimeClass* const TimeInfo);
  /**
   * \brief This is the default StockStdPrinter destructor
   */
  virtual ~StockStdPrinter();
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
   * \brief This is the name of the stock
   */
  char* stockname;
  /**
   * \brief This is the LengthGroupDivision of the stock
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the IntVector used to store aggregated area information
   */
  IntVector outerareas;
  /**
   * \brief This is the flag to denote whether the stock is a prey or not
   */
  int isaprey;
  /**
   * \brief This is the minimum age of the stock
   */
  int minage;
  /**
   * \brief This is the scaling factor used to scale the size of the stock
   */
  double scale;
  /**
   * \brief This is the StockAggregator used to collect information about the stock
   */
  StockAggregator* saggregator;
  /**
   * \brief This is the StockPreyAggregator used to collect information about the stock prey
   */
  StockPreyAggregator* paggregator;
  /**
   * \brief This is the PopStatistics used to calculate the statistics of the population
   */
  PopStatistics ps;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to temporarily store the information returned from the stock aggregatation function
   */
  const AgeBandMatrixPtrVector* salptr;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to temporarily store the information returned from the prey aggregatation function
   */
  const AgeBandMatrixPtrVector* palptr;
};

#endif
